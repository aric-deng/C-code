/*
 * arch/arm/mach-gk7101/gpio.c
 *
 * Author: Steven Yu, <yulindeng@gokemicro.com>
 * Copyright (C) 2014-2015, goke, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/device.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>

#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include <mach/hardware.h>
#include <mach/gpio.h>
#include <mach/io.h>

#define GPIO_REG(x)                   (GK7101_VA_GPIO0 + (x))

#define REG_GPIO_IS_LOW                GPIO_REG(0x0000) /* read/write */
#define REG_GPIO_IS_HIGH               GPIO_REG(0x0004) /* read/write */
#define REG_GPIO_IBE_LOW               GPIO_REG(0x0008) /* read/write */
#define REG_GPIO_IBE_HIGH              GPIO_REG(0x000C) /* read/write */
#define REG_GPIO_IEV_LOW               GPIO_REG(0x0010) /* read/write */
#define REG_GPIO_IEV_HIGH              GPIO_REG(0x0014) /* read/write */
#define REG_GPIO_IE_LOW                GPIO_REG(0x0018) /* read/write */
#define REG_GPIO_IE_HIGH               GPIO_REG(0x001C) /* read/write */
#define REG_GPIO_IC_LOW                GPIO_REG(0x0020) /* write */
#define REG_GPIO_IC_HIGH               GPIO_REG(0x0024) /* write */
#define REG_GPIO_RIS_LOW               GPIO_REG(0x0028) /* read */
#define REG_GPIO_RIS_HIGH              GPIO_REG(0x002C) /* read */
#define REG_GPIO_MIS_LOW               GPIO_REG(0x0030) /* read */
#define REG_GPIO_MIS_HIGH              GPIO_REG(0x0034) /* read */
#define REG_GPIO_INT_EN                GPIO_REG(0x0038) /* write */
#define REG_GPIO_PER_SEL               GPIO_REG(0x003C) /* read/write */
#define REG_GPIO_DIN_LOW               GPIO_REG(0x0040) /* read */
#define REG_GPIO_DIN_HIGH              GPIO_REG(0x0044) /* read */
#define REG_GPIO_OUTPUT_CFG            GPIO_REG(0x0100) /* write */
#define REG_GPIO_INPUT_CFG             GPIO_REG(0x0200) /* write */

#define REG_PLL_IOCTRL_GPIO            (GK7101_VA_RCT + 0x0264) /* read/write */

typedef union { /* PLL_IOCTRL_GPIO */
    u32 all;
    struct {
        u32 io1                         : 6;
        u32                             : 2;
        u32 io2                         : 6;
        u32                             : 2;
        u32 io0                         : 6;
        u32                             : 2;
        u32 io3                         : 6;
        u32                             : 2;
    } bitc;
} GH_PLL_IOCTRL_GPIO_S;

typedef union { /* GPIO_OUTPUT_CFG */
    u32 all;
    struct {
        u32 out_sel                     : 6;
        u32                             : 2;
        u32 oen_sel                     : 6;
        u32 out_invert                  : 1;
        u32 oen_invert                  : 1;
        u32                             : 16;
    } bitc;
} GH_GPIO_OUTPUT_CFG_S;

typedef union { /* GPIO_INPUT_CFG */
    u32 all;
    struct {
        u32 in_sel                      : 6;
        u32                             : 26;
    } bitc;
} GH_GPIO_INPUT_CFG_S;

typedef union { /* GPIO_INT_EN */
    u32 all;
    struct {
        u32 int_en                      : 1;
        u32                             : 31;
    } bitc;
} GH_GPIO_INT_EN_S;


u32                         m_gpio_ic_low;
u32                         m_gpio_ic_high;
GH_GPIO_INT_EN_S            m_gpio_int_en;
GH_GPIO_OUTPUT_CFG_S        m_gpio_output_cfg[64];
GH_GPIO_INPUT_CFG_S         m_gpio_input_cfg[64];

/* ==========================================================================*/
struct gk7101_gpio_chip
{
    struct gpio_chip    chip;
    u32         base_reg;
    spinlock_t      lock;
    struct
    {
        u32 per_sel_reg;
        u32 isl_reg;
        u32 ish_reg;
        u32 ibel_reg;
        u32 ibeh_reg;
        u32 ievl_reg;
        u32 ievh_reg;
        u32 iel_reg;
        u32 ieh_reg;
    }pm_info;
    u32                 irq0;
    u32                 irq1;
    struct gk7101_gpio_irq_info irq_info[64];
};
#define to_gk7101_gpio_chip(c) \
    container_of(c, struct gk7101_gpio_chip, chip)

int gk7101_gpio_set_type(u8 pin, GPIO_TYPE_E type);

static inline void gk7101_gpio_get_outcfg(u32 gpioid, u32* pvalue)
{
    *pvalue = gk_readl(GPIO0_OUTPUT_CFG_REG + (gpioid << 2));
}

static inline void gk7101_gpio_set_outcfg(u32 gpioid, u32 value)
{
    gk_writel((GPIO0_OUTPUT_CFG_REG + (gpioid << 2)), value);
}

static inline void gk7101_gpio_set_out(u32 gpioid, u32 value)
{
    if (value == GPIO_LOW) {
        gk_clrbitsl((GPIO0_OUTPUT_CFG_REG + (gpioid << 2)), 1);
    } else {
        gk_setbitsl((GPIO0_OUTPUT_CFG_REG + (gpioid << 2)), 1);
    }
}

void GH_PLL_set_IOCTRL_GPIO_io0(u8 index, u8 data)
{
    GH_PLL_IOCTRL_GPIO_S d;
    d.all = gk_readl(REG_PLL_IOCTRL_GPIO - (index * 0x00000004));
    d.bitc.io0 = data;
	gk_writel((REG_PLL_IOCTRL_GPIO - (index * 0x00000004)), d.all);
}

void GH_PLL_set_IOCTRL_GPIO_io1(u8 index, u8 data)
{
    GH_PLL_IOCTRL_GPIO_S d;
	d.all = gk_readl(REG_PLL_IOCTRL_GPIO - (index * 0x00000004));
    d.bitc.io1 = data;
	gk_writel((REG_PLL_IOCTRL_GPIO - (index * 0x00000004)), d.all);
}

void GH_PLL_set_IOCTRL_GPIO_io2(u8 index, u8 data)
{
    GH_PLL_IOCTRL_GPIO_S d;
	d.all = gk_readl(REG_PLL_IOCTRL_GPIO - (index * 0x00000004));
    d.bitc.io2 = data;
	gk_writel((REG_PLL_IOCTRL_GPIO - (index * 0x00000004)), d.all);
}

void GH_PLL_set_IOCTRL_GPIO_io3(u8 index, u8 data)
{
    GH_PLL_IOCTRL_GPIO_S d;
	d.all = gk_readl(REG_PLL_IOCTRL_GPIO - (index * 0x00000004));
    d.bitc.io3 = data;
	gk_writel((REG_PLL_IOCTRL_GPIO - (index * 0x00000004)), d.all);
}

static int gk7101_gpio_func_config(struct gk7101_gpio_chip *pgchip, u32 gpioid, u32 gtype)
{
    int             retval = 0;
    unsigned long   flags;
//    unsigned int    outreg;

//printk("gk7101_gpio_func_config: gpio %d , type 0x%x \n", gpioid, gtype);
    spin_lock_irqsave(&pgchip->lock, flags);

    //gk7101_gpio_get_outcfg(gpioid, &outreg);

    if(GPIO_GET_FUNC(gtype) > GPIO_FUNC_INOUT)
    {
        pr_err("%s: invalid GPIO func %d for GPIO:%d.\n", __func__, gtype, gpioid);
        spin_unlock_irqrestore(&pgchip->lock, flags);
        return -EINVAL;
    }

    retval =  gk7101_gpio_set_type(gpioid, gtype);
/*
    outreg &= (~(REG_OUTCFG_OEN_SELECT | REG_CONFIG_IN_OUT_SEL));
    outreg |= (GPIO_GET_OUT_SEL(gtype) | (GPIO_GET_OEN_SEL(gtype) << 8));

    if(GPIO_GET_OEN_INVERT(gtype))
    {
        outreg |= REG_OUTCFG_OEN_INVERT;
    }
    else
    {
        outreg &= (~(REG_OUTCFG_OEN_INVERT));
    }
    if(GPIO_GET_OUT_INVERT(gtype))
    {
        outreg |= REG_OUTCFG_OUT_INVERT;
    }
    else
    {
        outreg &= (~(REG_OUTCFG_OUT_INVERT));
    }

    if(GPIO_GET_FUNC(gtype) != GPIO_FUNC_OUT)
    {
        gk7101_gpio_set_incfg(GPIO_GET_FUNC(gtype) - 2, gpioid);
    }

    gk7101_gpio_set_outcfg(gpioid, outreg);

    if(GPIO_GET_IOCTRL(gtype))
    {
        switch(gpioid%4)
        {
        case 0:
            GH_PLL_set_IOCTRL_GPIO_io0(gpioid/4, GPIO_GET_IOCTRL(gtype));
            break;
        case 1:
            GH_PLL_set_IOCTRL_GPIO_io1(gpioid/4, GPIO_GET_IOCTRL(gtype));
            break;
        case 2:
            GH_PLL_set_IOCTRL_GPIO_io2(gpioid/4, GPIO_GET_IOCTRL(gtype));
            break;
        case 3:
            GH_PLL_set_IOCTRL_GPIO_io3(gpioid/4, GPIO_GET_IOCTRL(gtype));
            break;
        }
    }
*/
    spin_unlock_irqrestore(&pgchip->lock, flags);

    return retval;

}

static inline void gk7101_gpio_inline_set(
    struct gk7101_gpio_chip *agchip, u32 offset, int value)
{
    unsigned long               flags;

    spin_lock_irqsave(&agchip->lock, flags);
    gk7101_gpio_set_out(offset, value);
    spin_unlock_irqrestore(&agchip->lock, flags);
}

static inline int gk7101_gpio_inline_get(
    struct gk7101_gpio_chip *agchip, u32 offset)
{
    unsigned long               flags;
    u32 val = 0, gpioid;

    spin_lock_irqsave(&agchip->lock, flags);

    if(offset <= 31)
    {
        val = gk_readl(agchip->base_reg + GPIO_DIN_LOW_OFFSET);
        gpioid = offset;
    }
    else
    {
        gpioid = offset - 32;
        val = gk_readl(agchip->base_reg + GPIO_DIN_HIGH_OFFSET);
    }

    spin_unlock_irqrestore(&agchip->lock, flags);

    val = (val >> gpioid) & 0x1;
    return (val ? GPIO_HIGH : GPIO_LOW);

}

/* ==========================================================================*/
static DEFINE_MUTEX(gk7101_gpio_mtx);
static unsigned long gk7101_gpio_valid[BITS_TO_LONGS(GPIO_TOTAL_NUM)];
static unsigned long gk7101_gpio_freeflag[BITS_TO_LONGS(GPIO_TOTAL_NUM)];

static int gk7101_gpio_request(struct gpio_chip *chip, unsigned offset)
{
    int                 retval = 0;

    mutex_lock(&gk7101_gpio_mtx);

    if (test_bit((chip->base + offset), gk7101_gpio_valid)) {
        if (test_bit((chip->base + offset), gk7101_gpio_freeflag)) {
            __clear_bit((chip->base + offset),
                gk7101_gpio_freeflag);
        } else {
            retval = -EACCES;
        }
    } else {
        retval = -EPERM;
    }

    mutex_unlock(&gk7101_gpio_mtx);

    return retval;

}

static void gk7101_gpio_free(struct gpio_chip *chip, unsigned offset)
{
    mutex_lock(&gk7101_gpio_mtx);

    __set_bit((chip->base + offset), gk7101_gpio_freeflag);

    mutex_unlock(&gk7101_gpio_mtx);
}

static int gk7101_gpio_chip_direction_input(struct gpio_chip *chip,
    unsigned offset, int val)
{
    int                 retval = 0;
    struct gk7101_gpio_chip      *agchip;

    agchip = to_gk7101_gpio_chip(chip);
    if (val == 0)
        retval = gk7101_gpio_func_config(agchip, offset, GPIO_TYPE_INPUT_0);
    else
        retval = gk7101_gpio_func_config(agchip, offset, GPIO_TYPE_INPUT_1);
    return retval;
}

static int gk7101_gpio_chip_get(struct gpio_chip *chip,
    unsigned offset)
{
    int                 retval = 0;
    struct gk7101_gpio_chip      *agchip;

    agchip = to_gk7101_gpio_chip(chip);
    retval = gk7101_gpio_inline_get(agchip, offset);

    return retval;
}

static int gk7101_gpio_chip_direction_output(struct gpio_chip *chip,
    unsigned offset, int val)
{
    int                 retval = 0;
    struct gk7101_gpio_chip      *agchip;

    agchip = to_gk7101_gpio_chip(chip);
    retval = gk7101_gpio_func_config(agchip, offset, GPIO_TYPE_OUTPUT_0);
    gk7101_gpio_inline_set(agchip, offset, val);

    return retval;
}

static void gk7101_gpio_chip_set(struct gpio_chip *chip,
    unsigned offset, int val)
{
    struct gk7101_gpio_chip      *agchip;

    agchip = to_gk7101_gpio_chip(chip);
    gk7101_gpio_inline_set(agchip, offset, val);
}

static int gk7101_gpio_to_irq(struct gpio_chip *chip, unsigned offset)
{
    //if ((chip->base + offset) < GPIO_MAX_LINES)
    //    return GPIO_INT_VEC(chip->base + offset);
    return GPIO0_IRQ;

    return -EINVAL;
}

static void gk7101_gpio_chip_dbg_show(struct seq_file *s, struct gpio_chip *chip)
{
    int                 i;
    struct gk7101_gpio_chip      *agchip;
    u32                 afsel;
    u32                 lmask;
    u32                 data;
    u32                 hmask;
    unsigned long               flags;

    agchip = to_gk7101_gpio_chip(chip);

    spin_lock_irqsave(&agchip->lock, flags);
    afsel = gk_readl(agchip->base_reg + GPIO_PER_SEL_OFFSET);
    lmask = gk_readl(agchip->base_reg + GPIO_IE_LOW_OFFSET);
    hmask = gk_readl(agchip->base_reg + GPIO_IE_HIGH_OFFSET);
    data = gk_readl(agchip->base_reg + GPIO_INT_EN_OFFSET);
    spin_unlock_irqrestore(&agchip->lock, flags);

    seq_printf(s, "GPIO_BASE:\t0x%08X\n", agchip->base_reg);
    seq_printf(s, "GPIO_PSEL:\t0x%08X\n", afsel);
    seq_printf(s, "GPIO_MASK:\t0x%08X:0x%08X\n", hmask, lmask);
    seq_printf(s, "GPIO_GPEN:\t0x%08X\n", data);

    for (i = 0; i < chip->ngpio; i++) {
        seq_printf(s, "GPIO %d: ", (chip->base + i));
        seq_printf(s, "HW\n");
    }
}

#define GK7101_GPIO_BANK(name, reg_base, base_gpio)          \
{                                   \
    .chip = {                           \
        .label              = name,             \
        .owner              = THIS_MODULE,          \
        .request            = gk7101_gpio_request,   \
        .free               = gk7101_gpio_free,      \
        .direction_input    = gk7101_gpio_chip_direction_input,  \
        .get                = gk7101_gpio_chip_get,  \
        .direction_output   = gk7101_gpio_chip_direction_output, \
        .set                = gk7101_gpio_chip_set,  \
        .to_irq             = gk7101_gpio_to_irq,    \
        .dbg_show           = gk7101_gpio_chip_dbg_show, \
        .base               = base_gpio,            \
        .ngpio              = GPIO_BANK_SIZE,       \
        .can_sleep          = 0,                \
        .exported           = 0,                \
    },                              \
    .base_reg               = reg_base,         \
    .irq0                   = 0,    \
    .irq1                   = 0,    \
    .irq_info               = {0},  \
}

static struct gk7101_gpio_chip gk7101_gpio_banks[] = {
    GK7101_GPIO_BANK("gk7101-gpio0",
        GPIO0_BASE, GPIO(0 * GPIO_BANK_SIZE)),
};

/* ==========================================================================*/
static struct gk7101_gpio_chip *gk7101_gpio_id_to_chip(int id)
{
    struct gk7101_gpio_chip      *chip = NULL;

    if (id < 0) {
        chip = NULL;
    } else if (id < (1 * GPIO_BANK_SIZE)) {
        chip = &gk7101_gpio_banks[0];
    }

    return chip;
}

void gk7101_gpio_config(int id, u32 func)
{
    struct gk7101_gpio_chip      *chip;

    chip = gk7101_gpio_id_to_chip(id);
    if (chip == NULL) {
        pr_err("%s: invalid GPIO %d for func %d.\n", __func__, id, func);
        return;
    }

    if(gk7101_gpio_func_config(chip, (u32)id, func))
    {
        pr_err("%s: failed to configure GPIO %d for func %d.\n", __func__, id, func);
    }
}
EXPORT_SYMBOL(gk7101_gpio_config);

void gk7101_gpio_set(int id, int value)
{
    struct gk7101_gpio_chip      *chip;

    chip = gk7101_gpio_id_to_chip(id);
    if (chip == NULL) {
        pr_err("%s: invalid GPIO %d.\n", __func__, id);
        return;
    }

    gk7101_gpio_inline_set(chip, (u32)id, value);
}
EXPORT_SYMBOL(gk7101_gpio_set);

int gk7101_gpio_get(int id)
{
    struct gk7101_gpio_chip      *chip;

    chip = gk7101_gpio_id_to_chip(id);
    if (chip == NULL) {
        pr_err("%s: invalid GPIO %d.\n", __func__, id);
        return 0;
    }

    return gk7101_gpio_inline_get(chip, (u32)id);
}
EXPORT_SYMBOL(gk7101_gpio_get);

/* ==========================================================================*/
void gk7101_gpio_raw_lock(u32 id, unsigned long *pflags)
{
    spin_lock_irqsave(&gk7101_gpio_banks[id].lock, *pflags);
}

void gk7101_gpio_raw_unlock(u32 id, unsigned long *pflags)
{
    spin_unlock_irqrestore(&gk7101_gpio_banks[id].lock, *pflags);
}

#if 1
/* ==========================================================================*/
void __init gk7101_gpio_set_valid(unsigned pin, int valid)
{
    if (likely(pin >=0 && pin < GPIO_TOTAL_NUM)) {
        if (valid)
            __set_bit(pin, gk7101_gpio_valid);
        else
            __clear_bit(pin, gk7101_gpio_valid);
    }
}
#endif //modify by lhx

void GH_GPIO_set_OUTPUT_CFG_out_sel(u8 index, u8 data)
{
    m_gpio_output_cfg[index].bitc.out_sel = data;
	gk_writel((REG_GPIO_OUTPUT_CFG + (index * 0x4)), m_gpio_output_cfg[index].all);
    //*(volatile u32 *)(REG_GPIO_OUTPUT_CFG + (index * 0x4)) = m_gpio_output_cfg[index].all;
}

void GH_GPIO_set_OUTPUT_CFG_oen_sel(u8 index, u8 data)
{
    m_gpio_output_cfg[index].bitc.oen_sel = data;
	gk_writel((REG_GPIO_OUTPUT_CFG + (index * 0x4)), m_gpio_output_cfg[index].all);
    //*(volatile u32 *)(REG_GPIO_OUTPUT_CFG + (index * 0x4)) = m_gpio_output_cfg[index].all;
}

void GH_GPIO_set_INPUT_CFG_in_sel(u8 index, u8 data)
{
    m_gpio_input_cfg[index].bitc.in_sel = data;
	gk_writel((REG_GPIO_INPUT_CFG + (index * 0x4)), m_gpio_input_cfg[index].all);
    //*(volatile u32 *)(REG_GPIO_INPUT_CFG + (index * 0x4)) = m_gpio_input_cfg[index].all;
}

void GH_GPIO_set_OUTPUT_CFG_oen_invert(u8 index, u8 data)
{
    m_gpio_output_cfg[index].bitc.oen_invert = data;
	gk_writel((REG_GPIO_OUTPUT_CFG + (index * 0x4)), m_gpio_output_cfg[index].all);
    //*(volatile u32 *)(REG_GPIO_OUTPUT_CFG + (index * 0x4)) = m_gpio_output_cfg[index].all;
}

void GH_GPIO_set_OUTPUT_CFG_out_invert(u8 index, u8 data)
{
    m_gpio_output_cfg[index].bitc.out_invert = data;
	gk_writel((REG_GPIO_OUTPUT_CFG + (index * 0x4)), m_gpio_output_cfg[index].all);
    //*(volatile u32 *)(REG_GPIO_OUTPUT_CFG + (index * 0x4)) = m_gpio_output_cfg[index].all;
}

int gk7101_gpio_set_type(u8 pin, GPIO_TYPE_E type)
{
//printk("gpio %-2d set type to 0x%x  \n", pin, type);
    switch(GPIO_GET_FUNC(type))
    {
    case GPIO_FUNC_OUT:     // out
        GH_GPIO_set_OUTPUT_CFG_out_sel(pin, GPIO_GET_OUT_SEL(type));
        GH_GPIO_set_OUTPUT_CFG_oen_sel(pin, GPIO_GET_OEN_SEL(type));
        break;
    case GPIO_FUNC_IN:     // in
        if(GPIO_GET_IN_SEL(type) >= 2)
        {
            GH_GPIO_set_OUTPUT_CFG_out_sel(pin, GPIO_GET_OUT_SEL(type));
            GH_GPIO_set_OUTPUT_CFG_oen_sel(pin, GPIO_GET_OEN_SEL(type));
            GH_GPIO_set_INPUT_CFG_in_sel(GPIO_GET_IN_SEL(type) - 2, pin);
        }
        else
        {
            GH_GPIO_set_OUTPUT_CFG_out_sel(pin, GPIO_GET_OUT_SEL(type));
            GH_GPIO_set_OUTPUT_CFG_oen_sel(pin, GPIO_GET_OEN_SEL(type));
            //GH_GPIO_set_INPUT_CFG_in_sel(GPIO_GET_IN_SEL(type) - 2, pin);
        }
        break;
    case GPIO_FUNC_INOUT:     // in+out
        // don't change, otherwise if out_sel at first might output a 0, then change to 1
        GH_GPIO_set_INPUT_CFG_in_sel(GPIO_GET_IN_SEL(type) - 2, pin);
        GH_GPIO_set_OUTPUT_CFG_oen_sel(pin, GPIO_GET_OEN_SEL(type));
        GH_GPIO_set_OUTPUT_CFG_out_sel(pin, GPIO_GET_OUT_SEL(type));
        break;
    default:
        return -EINVAL;
    }
    GH_GPIO_set_OUTPUT_CFG_oen_invert(pin, GPIO_GET_OEN_INVERT(type));
    GH_GPIO_set_OUTPUT_CFG_out_invert(pin, GPIO_GET_OUT_INVERT(type));
    // Pull up/down & 2mA......
    if(GPIO_GET_IOCTRL(type))
    {
        switch(pin%4)
        {
        case 0:
            GH_PLL_set_IOCTRL_GPIO_io0(pin/4, GPIO_GET_IOCTRL(type));
            break;
        case 1:
            GH_PLL_set_IOCTRL_GPIO_io1(pin/4, GPIO_GET_IOCTRL(type));
            break;
        case 2:
            GH_PLL_set_IOCTRL_GPIO_io2(pin/4, GPIO_GET_IOCTRL(type));
            break;
        case 3:
            GH_PLL_set_IOCTRL_GPIO_io3(pin/4, GPIO_GET_IOCTRL(type));
            break;
        }
    }

    return(0);
}

extern uint8_t cmdline_phytype;
int __init gk7101_init_gpio(void)
{
    int     retval = 0;
    int     i, index;
    int     gpio_count;

    GPIO_XREF_S gpio_xref_table[] = { SYSTEM_GPIO_XREF_TABLE };
    #ifdef SYSTEM_GPIO_EXTPHY_TABLE
    GPIO_XREF_S ext_phy_gpio_xref_table[] = { SYSTEM_GPIO_EXTPHY_TABLE };
    #endif
    GPIO_XREF_S int_phy_gpio_xref_table[] = { SYSTEM_GPIO_INTPHY_TABLE };
	GPIO_XREF_S cmd_gpio_xref_table[] = { SYSTEM_CONFIRM_TABLE };
    gpio_count = sizeof(gpio_xref_table)/sizeof(gpio_xref_table[0]);

    mutex_lock(&gk7101_gpio_mtx);
	memset(gk7101_gpio_valid, 0xff, sizeof(gk7101_gpio_valid));
	memset(gk7101_gpio_freeflag, 0xff, sizeof(gk7101_gpio_freeflag));
	for (i = GPIO_MAX_LINES + 1; i < GPIO_TOTAL_NUM; i++) {
		gk7101_gpio_set_valid(i, 0);
		__clear_bit(i, gk7101_gpio_freeflag);
	}
    mutex_unlock(&gk7101_gpio_mtx);

    for (i = 0; i < ARRAY_SIZE(gk7101_gpio_banks); i++) {
        spin_lock_init(&gk7101_gpio_banks[i].lock);
        retval = gpiochip_add(&gk7101_gpio_banks[i].chip);
        if (retval) {
            pr_err("%s: gpiochip_add %s fail %d.\n", __func__,
                gk7101_gpio_banks[i].chip.label, retval);
            break;
        }
    }

    printk("gpio map init...\n");
    for(index=0; index < gpio_count; index++)
    {
        if(gpio_xref_table[index].type != GPIO_TYPE_UNDEFINED)
        {
            gk7101_gpio_set_type(gpio_xref_table[index].pin, gpio_xref_table[index].type);
        }
        //else printk("gpio %d undefined  \n", gpio_xref_table[index].pin);
    }
	
//    printk("...........cmdline_phytype=%d...... \n", cmdline_phytype);
	if(cmdline_phytype == 0){
		gpio_count = sizeof(int_phy_gpio_xref_table)/sizeof(int_phy_gpio_xref_table[0]);
	    for(index=0; index < gpio_count; index++)
	    {
	        if(int_phy_gpio_xref_table[index].type != GPIO_TYPE_UNDEFINED)
	        {
	            gk7101_gpio_set_type(int_phy_gpio_xref_table[index].pin, int_phy_gpio_xref_table[index].type);
	        }
	        //else printk("gpio %d undefined  \n", gpio_xref_table[index].pin);
	    }
	}else{
	#ifdef SYSTEM_GPIO_EXTPHY_TABLE_
		gpio_count = sizeof(ext_phy_gpio_xref_table)/sizeof(ext_phy_gpio_xref_table[0]);
	    for(index=0; index < gpio_count; index++)
	    {
	        if(ext_phy_gpio_xref_table[index].type != GPIO_TYPE_UNDEFINED)
	        {
	            gk7101_gpio_set_type(ext_phy_gpio_xref_table[index].pin, ext_phy_gpio_xref_table[index].type);
	        }
	        //else printk("gpio %d undefined  \n", gpio_xref_table[index].pin);
	    }//  #error
     #endif
	/* gpio_count = sizeof(cmd_gpio_xref_table)/sizeof(cmd_gpio_xref_table[0]);
		     for(index=0; index < gpio_count; index++)
			    {
			        if(cmd_gpio_xref_table[index].type != GPIO_TYPE_UNDEFINED)
			        {
			            gk7101_gpio_set_type(cmd_gpio_xref_table[index].pin, cmd_gpio_xref_table[index].type);
			        }
			    }*/
	}
    //GH_GPIO_set_INT_EN(1);
  //  if(cmdline_phytype !=  0)
//	 gk7101_gpio_set_type(GPIO_49, GPIO_TYPE_INPUT_0);
    gk_writel(REG_GPIO_INT_EN, 0x00000001);
    //GH_GPIO_set_PER_SEL((u32)0x00000003);

	return retval;
}

/* ==========================================================================*/
u32 gk7101_gpio_suspend(u32 level)
{
    u32                 i;
    unsigned long               flags;

    for (i = 0; i < GPIO_INSTANCES; i++)
    {
        spin_lock_irqsave(&gk7101_gpio_banks[i].lock, flags);
        gk7101_gpio_banks[i].pm_info.per_sel_reg    = gk_readl(GPIO0_PER_SEL_REG);
        gk7101_gpio_banks[i].pm_info.isl_reg        = gk_readl(GPIO0_IS_LOW_REG);
        gk7101_gpio_banks[i].pm_info.ish_reg        = gk_readl(GPIO0_IS_HIGH_REG);
        gk7101_gpio_banks[i].pm_info.ibel_reg       = gk_readl(GPIO0_IBE_LOW_REG);
        gk7101_gpio_banks[i].pm_info.ibeh_reg       = gk_readl(GPIO0_IBE_HIGH_REG);
        gk7101_gpio_banks[i].pm_info.ievl_reg       = gk_readl(GPIO0_IEV_LOW_REG);
        gk7101_gpio_banks[i].pm_info.ievh_reg       = gk_readl(GPIO0_IEV_HIGH_REG);
        gk7101_gpio_banks[i].pm_info.iel_reg        = gk_readl(GPIO0_IE_LOW_REG);
        gk7101_gpio_banks[i].pm_info.ieh_reg        = gk_readl(GPIO0_IE_HIGH_REG);
        spin_unlock_irqrestore(&gk7101_gpio_banks[i].lock, flags);
    }

    gk_writel(GPIO0_INT_EN_REG, 0x00000000);

    return 0;

}

u32 gk7101_gpio_resume(u32 level)
{
    u32                 i;
    unsigned long               flags;

    for (i = 0; i < GPIO_INSTANCES; i++)
    {
        spin_lock_irqsave(&gk7101_gpio_banks[i].lock, flags);
        gk_writel(GPIO0_PER_SEL_REG , gk7101_gpio_banks[i].pm_info.per_sel_reg);
        gk_writel(GPIO0_IS_LOW_REG  , gk7101_gpio_banks[i].pm_info.isl_reg  );
        gk_writel(GPIO0_IS_HIGH_REG , gk7101_gpio_banks[i].pm_info.ish_reg  );
        gk_writel(GPIO0_IBE_LOW_REG , gk7101_gpio_banks[i].pm_info.ibel_reg );
        gk_writel(GPIO0_IBE_HIGH_REG, gk7101_gpio_banks[i].pm_info.ibeh_reg );
        gk_writel(GPIO0_IEV_LOW_REG , gk7101_gpio_banks[i].pm_info.ievl_reg );
        gk_writel(GPIO0_IEV_HIGH_REG, gk7101_gpio_banks[i].pm_info.ievh_reg );
        gk_writel(GPIO0_IE_LOW_REG  , gk7101_gpio_banks[i].pm_info.iel_reg  );
        gk_writel(GPIO0_IE_HIGH_REG , gk7101_gpio_banks[i].pm_info.ieh_reg  );
        spin_unlock_irqrestore(&gk7101_gpio_banks[i].lock, flags);
    }

    gk_writel(GPIO0_INT_EN_REG, 1);
    return 0;

}

/* ==========================================================================*/
int gk7101_set_gpio_output_can_sleep(
    struct gk7101_gpio_io_info *pinfo, u32 on, int can_sleep)
{
    int                 retval = 0;

    if (pinfo == NULL) {
        pr_err("%s: pinfo is NULL.\n", __func__);
        retval = -1;
        goto gk7101_set_gpio_output_can_sleep_exit;
    }
    if (pinfo->gpio_id < 0 ) {
        pr_debug("%s: wrong gpio id %d.\n", __func__, pinfo->gpio_id);
        retval = -1;
        goto gk7101_set_gpio_output_can_sleep_exit;
    }

    pr_debug("%s: Gpio[%d] %s, level[%s], delay[%dms].\n", __func__,
        pinfo->gpio_id, on ? "ON" : "OFF",
        pinfo->active_level ? "HIGH" : "LOW",
        pinfo->active_delay);
    if (pinfo->gpio_id >= GPIO_TOTAL_NUM)
    {
        pr_debug("%s: try expander gpio id %d.\n",
            __func__, pinfo->gpio_id);
        retval = -1;
        goto gk7101_set_gpio_output_can_sleep_exit;
    }
    else
    {
        if (on) {
            gk7101_gpio_config(pinfo->gpio_id, pinfo->active_level ? GPIO_TYPE_OUTPUT_1 : GPIO_TYPE_OUTPUT_0);
            gk7101_gpio_set(pinfo->gpio_id,
                pinfo->active_level);
        } else {
            gk7101_gpio_config(pinfo->gpio_id, pinfo->active_level ? GPIO_TYPE_OUTPUT_0 : GPIO_TYPE_OUTPUT_1);
            gk7101_gpio_set(pinfo->gpio_id,
                !pinfo->active_level);
        }
    }
    if (can_sleep) {
        msleep(pinfo->active_delay);
    } else {
        mdelay(pinfo->active_delay);
    }

gk7101_set_gpio_output_can_sleep_exit:
    return retval;
}
EXPORT_SYMBOL(gk7101_set_gpio_output_can_sleep);

u32 gk7101_get_gpio_input_can_sleep(
    struct gk7101_gpio_io_info *pinfo, int can_sleep)
{
    u32                 gpio_value = 0;

    if (pinfo == NULL) {
        pr_err("%s: pinfo is NULL.\n", __func__);
        goto gk7101_get_gpio_input_can_sleep_exit;
    }
    if (pinfo->gpio_id < 0 ) {
        pr_debug("%s: wrong gpio id %d.\n", __func__, pinfo->gpio_id);
        goto gk7101_get_gpio_input_can_sleep_exit;
    }

    if (pinfo->gpio_id >= GPIO_TOTAL_NUM)
    {
        pr_debug("%s: try expander gpio id %d.\n",
            __func__, pinfo->gpio_id);
        goto gk7101_get_gpio_input_can_sleep_exit;
    }
    else
    {
        gk7101_gpio_config(pinfo->gpio_id, pinfo->active_level ? GPIO_TYPE_OUTPUT_1 : GPIO_TYPE_OUTPUT_0);
        if (can_sleep) {
            msleep(pinfo->active_delay);
        } else {
            mdelay(pinfo->active_delay);
        }
        gpio_value = gk7101_gpio_get(pinfo->gpio_id);
    }

    pr_debug("%s: {gpio[%d], level[%s], delay[%dms]} get[%d].\n",
        __func__, pinfo->gpio_id,
        pinfo->active_level ? "HIGH" : "LOW",
        pinfo->active_delay, gpio_value);

gk7101_get_gpio_input_can_sleep_exit:
    return (gpio_value == pinfo->active_level) ? 1 : 0;
}
EXPORT_SYMBOL(gk7101_get_gpio_input_can_sleep);

int gk7101_set_gpio_reset_can_sleep(
    struct gk7101_gpio_io_info *pinfo, int can_sleep)
{
    int                 retval = 0;

    if (pinfo == NULL) {
        pr_err("%s: pinfo is NULL.\n", __func__);
        retval = -1;
        goto gk7101_set_gpio_reset_can_sleep_exit;
    }
    if (pinfo->gpio_id < 0 ) {
        pr_debug("%s: wrong gpio id %d.\n", __func__, pinfo->gpio_id);
        retval = -1;
        goto gk7101_set_gpio_reset_can_sleep_exit;
    }

    pr_debug("%s: Reset gpio[%d], level[%s], delay[%dms].\n",
        __func__, pinfo->gpio_id,
        pinfo->active_level ? "HIGH" : "LOW",
        pinfo->active_delay);
    if (pinfo->gpio_id >= GPIO_TOTAL_NUM) {
        pr_debug("%s: try expander gpio id %d.\n",
            __func__, pinfo->gpio_id);
        retval = -1;
        goto gk7101_set_gpio_reset_can_sleep_exit;
    } else {
        gk7101_gpio_config(pinfo->gpio_id, GPIO_TYPE_OUTPUT_0);
        gk7101_gpio_set(pinfo->gpio_id, pinfo->active_level);
        if (can_sleep) {
            msleep(pinfo->active_delay);
        } else {
            mdelay(pinfo->active_delay);
        }
        gk7101_gpio_set(pinfo->gpio_id, !pinfo->active_level);
        if (can_sleep) {
            msleep(pinfo->active_delay);
        } else {
            mdelay(pinfo->active_delay);
        }
    }

gk7101_set_gpio_reset_can_sleep_exit:
    return retval;
}
EXPORT_SYMBOL(gk7101_set_gpio_reset_can_sleep);

/* ==========================================================================*/
int gk7101_set_gpio_output(struct gk7101_gpio_io_info *pinfo, u32 on)
{
    return gk7101_set_gpio_output_can_sleep(pinfo, on, 0);
}
EXPORT_SYMBOL(gk7101_set_gpio_output);

u32 gk7101_get_gpio_input(struct gk7101_gpio_io_info *pinfo)
{
    return gk7101_get_gpio_input_can_sleep(pinfo, 0);
}
EXPORT_SYMBOL(gk7101_get_gpio_input);

int gk7101_set_gpio_reset(struct gk7101_gpio_io_info *pinfo)
{
    return gk7101_set_gpio_reset_can_sleep(pinfo, 0);
}
EXPORT_SYMBOL(gk7101_set_gpio_reset);

void gk7101_gpio_set_out_ext(u32 id, u32 value)
{
    if (value == GPIO_LOW) {
        gk_clrbitsl((GPIO0_OUTPUT_CFG_REG + (id << 2)), 1);
    } else {
        gk_setbitsl((GPIO0_OUTPUT_CFG_REG + (id << 2)), 1);
    }
}
EXPORT_SYMBOL(gk7101_gpio_set_out_ext);

int gk7101_is_valid_gpio_irq(struct gk7101_gpio_irq_info *pinfo)
{
    int                 bvalid = 0;
//MY_DEBUG();
    if (pinfo == NULL) {
        printk("%s: pinfo is NULL.\n", __func__);
        return 0;
    }
//printk("gpio[%d] type[%d] \n", pinfo->irq_gpio, pinfo->irq_type);

    if ((pinfo->irq_gpio < 0 ) || (pinfo->irq_gpio >= ARCH_NR_GPIOS))
        return 0;

    if ((pinfo->irq_type != IRQ_TYPE_EDGE_RISING) &&
        (pinfo->irq_type != IRQ_TYPE_EDGE_FALLING) &&
        (pinfo->irq_type != IRQ_TYPE_EDGE_BOTH) &&
        (pinfo->irq_type != IRQ_TYPE_LEVEL_HIGH) &&
        (pinfo->irq_type != IRQ_TYPE_LEVEL_LOW))
        return 0;

//printk("gpio val[%d] gpio_mode[%d] \n", pinfo->irq_gpio_val, pinfo->irq_gpio_mode);
    if ((pinfo->irq_gpio_val != GPIO_HIGH) &&
        (pinfo->irq_gpio_val != GPIO_LOW))
        return 0;

    if (pinfo->irq_gpio_mode > GPIO_FUNC_INOUT)
        return 0;

//printk("irq_line[%d]   >=%d  < %d\n", pinfo->irq_line, NR_IRQS, GPIO_INT_VEC(0));
    if (((pinfo->irq_line < GPIO_INT_VEC(0)) ||
       (pinfo->irq_line >= NR_IRQS)))
        return 0;

    bvalid = 1;
//    printk("gpio valid = %d\n", bvalid);
    return 1;
}
EXPORT_SYMBOL(gk7101_is_valid_gpio_irq);

static irqreturn_t gk7101_gpio_irq(int irq, void *dev)
{
    struct gk7101_gpio_chip     *chip;
    struct gk7101_gpio_irq_info *pinfo = (struct gk7101_gpio_irq_info*)dev;
    unsigned long   flags;
    u32 misl_reg;
    u32 mish_reg;
    u32 index;
    u32 mask = 1;

    mdelay(20);

    if (pinfo == NULL)
    {
        pr_err("%s: pinfo is NULL.\n", __func__);
        return IRQ_NONE;
    }
    chip = gk7101_gpio_id_to_chip(pinfo->irq_gpio);
    if (chip == NULL)
    {
        pr_err("%s: invalid GPIO %d.\n", __func__, pinfo->irq_gpio);
        return IRQ_NONE;
    }
    spin_lock_irqsave(&chip->lock, flags);
    misl_reg    = gk_readl(GPIO0_MIS_LOW_REG);
    mish_reg    = gk_readl(GPIO0_MIS_HIGH_REG);
    gk_writel(GPIO0_IC_LOW_REG, misl_reg);
    gk_writel(GPIO0_IC_HIGH_REG, mish_reg);

    spin_unlock_irqrestore(&chip->lock, flags);
    for(index=0; index < 32; index++)
    {
        if(misl_reg & mask)
        {
            pinfo[index].irq_gpio_val = gk7101_gpio_inline_get(chip, pinfo[index].irq_gpio);
            if (pinfo[index].handler)
            {
                pinfo[index].handler(irq, &pinfo[index]);
            }
        }
        if(mish_reg & mask)
        {
            pinfo[index + 32].irq_gpio_val = gk7101_gpio_inline_get(chip, pinfo[index + 32].irq_gpio);
            if (pinfo[index + 32].handler)
            {
                pinfo[index + 32].handler(irq, &pinfo[index + 32]);
            }
        }
        mask <<= 1;
    }
    return IRQ_HANDLED;
}

int gk7101_gpio_request_irq(struct gk7101_gpio_irq_info *pinfo)
{
    struct gk7101_gpio_chip     *chip;
    u32 mask;
    unsigned long   flags;
    if (pinfo == NULL)
    {
        pr_err("%s: pinfo is NULL.\n", __func__);
        return -1;
    }

    chip = gk7101_gpio_id_to_chip(pinfo->irq_gpio);
    if (chip == NULL)
    {
        pr_err("%s: invalid GPIO %d.\n", __func__, pinfo->irq_gpio);
        return -1;
    }
    spin_lock_irqsave(&chip->lock, flags);
    memcpy(&chip->irq_info[pinfo->irq_gpio], pinfo, sizeof(struct gk7101_gpio_irq_info));
    gk_writel(GPIO0_INT_EN_REG, 0x00);
    mask = 0x01<<(pinfo->irq_gpio % 0x20);
    if((chip->irq0 == 0x00) && (chip->irq1 == 0x00))
    {
        request_irq(GPIO0_IRQ, gk7101_gpio_irq, IRQF_TRIGGER_HIGH, "gpio_irq", (void*)&chip->irq_info[0]);
    }
    if(pinfo->irq_gpio / 0x20)
    {
        chip->pm_info.ish_reg        = gk_readl(GPIO0_IS_HIGH_REG);
        chip->pm_info.ibeh_reg       = gk_readl(GPIO0_IBE_HIGH_REG);
        chip->pm_info.ievh_reg       = gk_readl(GPIO0_IEV_HIGH_REG);
        chip->pm_info.ieh_reg        = gk_readl(GPIO0_IE_HIGH_REG);

        chip->pm_info.ieh_reg       |= mask;
        chip->irq1 = chip->pm_info.ieh_reg;
        switch(pinfo->irq_type)
        {
        case IRQ_TYPE_LEVEL_LOW:
            chip->pm_info.ish_reg        |= mask;
            chip->pm_info.ibeh_reg       &= ~mask;
            chip->pm_info.ievh_reg       &= ~mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_1);
            break;
        case IRQ_TYPE_LEVEL_HIGH:
            chip->pm_info.ish_reg        |= mask;
            chip->pm_info.ibeh_reg       &= ~mask;
            chip->pm_info.ievh_reg       |= mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_0);
            break;
        case IRQ_TYPE_EDGE_FALLING:
            chip->pm_info.ish_reg        &= ~mask;
            chip->pm_info.ibeh_reg       &= ~mask;
            chip->pm_info.ievh_reg       &= ~mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_1);
            break;
        case IRQ_TYPE_EDGE_RISING:
            chip->pm_info.ish_reg        &= ~mask;
            chip->pm_info.ibeh_reg       &= ~mask;
            chip->pm_info.ievh_reg       |= mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_0);
            break;
        case IRQ_TYPE_EDGE_BOTH:
            chip->pm_info.ish_reg        &= ~mask;
            chip->pm_info.ibeh_reg       |= mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_1);
            break;
        }
        gk_writel(GPIO0_IS_HIGH_REG, chip->pm_info.ish_reg);
        gk_writel(GPIO0_IBE_HIGH_REG, chip->pm_info.ibeh_reg);
        gk_writel(GPIO0_IEV_HIGH_REG, chip->pm_info.ievh_reg);
        gk_writel(GPIO0_IE_HIGH_REG, chip->pm_info.ieh_reg);
    }
    else
    {
        chip->pm_info.isl_reg        = gk_readl(GPIO0_IS_LOW_REG);
        chip->pm_info.ibel_reg       = gk_readl(GPIO0_IBE_LOW_REG);
        chip->pm_info.ievl_reg       = gk_readl(GPIO0_IEV_LOW_REG);
        chip->pm_info.iel_reg        = gk_readl(GPIO0_IE_LOW_REG);

        chip->pm_info.iel_reg       |= mask;
        chip->irq0 = chip->pm_info.iel_reg;
        switch(pinfo->irq_type)
        {
        case IRQ_TYPE_LEVEL_LOW:
            chip->pm_info.isl_reg        |= mask;
            chip->pm_info.ibel_reg       &= ~mask;
            chip->pm_info.ievl_reg       &= ~mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_1);
            break;
        case IRQ_TYPE_LEVEL_HIGH:
            chip->pm_info.isl_reg        |= mask;
            chip->pm_info.ibel_reg       &= ~mask;
            chip->pm_info.ievl_reg       |= mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_0);
            break;
        case IRQ_TYPE_EDGE_FALLING:
            chip->pm_info.isl_reg        &= ~mask;
            chip->pm_info.ibel_reg       &= ~mask;
            chip->pm_info.ievl_reg       &= ~mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_1);
            break;
        case IRQ_TYPE_EDGE_RISING:
            chip->pm_info.isl_reg        &= ~mask;
            chip->pm_info.ibel_reg       &= ~mask;
            chip->pm_info.ievl_reg       |= mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_0);
            break;
        case IRQ_TYPE_EDGE_BOTH:
            chip->pm_info.isl_reg        &= ~mask;
            chip->pm_info.ibel_reg       |= mask;
            gk7101_gpio_set_type(pinfo->irq_gpio, GPIO_TYPE_INPUT_1);
            break;
        }
        gk_writel(GPIO0_IS_LOW_REG, chip->pm_info.isl_reg);
        gk_writel(GPIO0_IBE_LOW_REG, chip->pm_info.ibel_reg);
        gk_writel(GPIO0_IEV_LOW_REG, chip->pm_info.ievl_reg);
        gk_writel(GPIO0_IE_LOW_REG, chip->pm_info.iel_reg);
    }
    gk_writel(GPIO0_INT_EN_REG, 0x01);
    spin_unlock_irqrestore(&chip->lock, flags);
    return 0;
}
EXPORT_SYMBOL(gk7101_gpio_request_irq);

int gk7101_gpio_release_irq(GPIO_PIN_E pin)
{
    struct gk7101_gpio_chip     *chip;
    u32 mask;
    unsigned long   flags;

    chip = gk7101_gpio_id_to_chip(pin);
    if (chip == NULL)
    {
        pr_err("%s: invalid GPIO %d.\n", __func__, pin);
        return -1;
    }
    spin_lock_irqsave(&chip->lock, flags);
    memset(&chip->irq_info[pin], 0x00, sizeof(struct gk7101_gpio_irq_info));
    mask = 0x01<<(pin % 0x20);

    if(pin / 0x20)
    {
        chip->pm_info.ieh_reg        = gk_readl(GPIO0_IE_HIGH_REG);
        chip->pm_info.ieh_reg       &= ~mask;
        chip->irq1 = chip->pm_info.ieh_reg;
        gk_writel(GPIO0_IE_HIGH_REG, chip->pm_info.ieh_reg);
    }
    else
    {
        chip->pm_info.iel_reg        = gk_readl(GPIO0_IE_LOW_REG);
        chip->pm_info.iel_reg       &= ~mask;
        chip->irq0 = chip->pm_info.iel_reg;
        gk_writel(GPIO0_IE_LOW_REG, chip->pm_info.iel_reg);
    }
    if((chip->irq0 == 0x00) && (chip->irq1 == 0x00))
    {
        gk_writel(GPIO0_INT_EN_REG, 0x00);
    }
    spin_unlock_irqrestore(&chip->lock, flags);
    return 0;
}
EXPORT_SYMBOL(gk7101_gpio_release_irq);

int gk7101_set_ircut(u32 mode)
{
    // A=SYSTEM_GPIO_IR_CUT2
    // B=SYSTEM_GPIO_IR_CUT1
    if(mode==0) // clear A clear B
    {
        gk7101_gpio_set_out(SYSTEM_GPIO_IR_CUT1, 0);
        gk7101_gpio_set_out(SYSTEM_GPIO_IR_CUT2, 0);
    }
    else if(mode==1) // night set A clear B
    {
        gk7101_gpio_set_out(SYSTEM_GPIO_IR_CUT1, 0);
        gk7101_gpio_set_out(SYSTEM_GPIO_IR_CUT2, 1);
    }
    else if(mode==2) // day clear A set B
    {
        gk7101_gpio_set_out(SYSTEM_GPIO_IR_CUT1, 1);
        gk7101_gpio_set_out(SYSTEM_GPIO_IR_CUT2, 0);
    }
    else
    {
        return -1;
    }
    return 0;
}
EXPORT_SYMBOL(gk7101_set_ircut);

