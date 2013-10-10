/*
 * Export interface in /sys/class/touchpanel for calibration.
 *
 * Yongle Lai @ Rockchip - 2010-07-26
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/input.h>

#include "calibration_ts.h"

/*
 * The sys nodes for touch panel calibration depends on controller's name,
 * such as: /sys/bus/spi/drivers/xpt2046_ts/touchadc
 * If we use another TP controller (not xpt2046_ts), the above path will 
 * be unavailable which will cause calibration to be fail.
 *
 * Another choice is: 
 *   sys/devices/platform/rockchip_spi_master/spi0.0/driver/touchadc
 * this path request the TP controller will be the first device of SPI.
 *
 * To make TP calibration module in Android be universal, we create
 * a class named touchpanel as the path for calibration interfaces.
 */
 
/*
 * TPC driver depended.
 */
extern volatile struct adc_point gADPoint;
#ifdef TS_PRESSURE
extern volatile int gZvalue[3];
#endif

#if defined(CONFIG_TOUCHSCREEN_800X600)
#ifdef CONFIG_LANDSCAPE_DISPLAY
int screen_x[5] = { 50, 750,  50, 750, 400};
int screen_y[5] = { 40,  40, 560, 560, 300};
#elif defined (CONFIG_PORTRAIT_DISPLAY)
int screen_x[5] = { 50,  50, 770, 770,  300};
int screen_y[5] = { 557,  37, 557, 37,  400};
#endif
#elif defined(CONFIG_TOUCHSCREEN_800X480)
#ifdef CONFIG_LANDSCAPE_DISPLAY
int screen_x[5] = { 50, 750,  50, 750, 400};
int screen_y[5] = { 40,  40, 440, 440, 240};
#elif defined (CONFIG_PORTRAIT_DISPLAY)
int screen_x[5] = { 50,  50, 750, 750,  400};
int screen_y[5] = { 440,  40, 440, 40,  240};
#endif
#else
int screen_x[5] = { 0 };
int screen_y[5] = { 0 };
#endif

#if defined (CONFIG_MACH_M722HR) || defined(CONFIG_MACH_M723HR)
int uncali_x_default[5] = {902, 113, 909, 114, 521};
int uncali_y_default[5] = {813, 826, 183, 177, 493};
#elif defined (CONFIG_MACH_M911)
int uncali_x_default[5] = {904, 899, 102, 107, 803};
int uncali_y_default[5] = {201, 830, 215, 850, 536};

//int uncali_x_default[5] = {907, 903, 99, 105, 559};
//int uncali_y_default[5] = {204, 835, 212, 847, 508};

//int uncali_x_default[5] = {842, 173, 839, 166, 512};
//int uncali_y_default[5] = {120, 122, 915, 913, 393};
#else
int uncali_x_default[5] = {195, 204, 823, 840, 531};
int uncali_y_default[5] = {115, 908, 128, 916, 496};
#endif

int uncali_x[5] = { 0 };
int uncali_y[5] = { 0 };

static ssize_t touch_mode_show(struct device *dev, struct device_attribute *attr, char *_buf)
{
    int count;
    
	count = sprintf(_buf,"TouchCheck:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	                uncali_x[0], uncali_y[0],
	                uncali_x[1], uncali_y[1],
	                uncali_x[2], uncali_y[2],
	                uncali_x[3], uncali_y[3],
	                uncali_x[4], uncali_y[4]);

	printk("buf: %s", _buf);
		
	return count;
}

static ssize_t touch_mode_store(struct device *dev, struct device_attribute *attr, const char *_buf, size_t _count)
{
    int i, j = 0;
    char temp[5];

    //printk("Read data from Android: %s\n", _buf);
    
    for (i = 0; i < 5; i++)
    {
        strncpy(temp, _buf + 5 * (j++), 4);
        uncali_x[i] = simple_strtol(temp, NULL, 10);
        strncpy(temp, _buf + 5 * (j++), 4);
        uncali_y[i] = simple_strtol(temp, NULL, 10);
        printk("SN=%d uncali_x=%d uncali_y=%d\n", 
                i, uncali_x[i], uncali_y[i]);
    }

    return _count; 
}

//This code is Touch adc simple value
static ssize_t touch_adc_show(struct device *dev, struct device_attribute *attr, char *_buf)
{
    printk("ADC show: x=%d y=%d\n", gADPoint.x, gADPoint.y);
    
	return sprintf(_buf, "%d,%d\n", gADPoint.x, gADPoint.y);
}

static ssize_t touch_cali_status(struct device *dev, struct device_attribute *attr, char *_buf)
{
    int ret;
    
    ret = TouchPanelSetCalibration(4, screen_x, screen_y, uncali_x, uncali_y);
    if (ret == 1){
    	memcpy(uncali_x_default, uncali_x, sizeof(uncali_x));
    	memcpy(uncali_y_default, uncali_y, sizeof(uncali_y));
    	printk("touch_cali_status-0--%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	                uncali_x_default[0], uncali_y_default[0],
	                uncali_x_default[1], uncali_y_default[1],
	                uncali_x_default[2], uncali_y_default[2],
	                uncali_x_default[3], uncali_y_default[3],
	                uncali_x_default[4], uncali_y_default[4]);
    	ret = sprintf(_buf, "successful\n");
    }
    else{
     	printk("touchpal calibration failed, use default value.\n");
#if 0
    	ret = TouchPanelSetCalibration(4, screen_x, screen_y, uncali_x_default, uncali_y_default);
    	printk("touch_cali_status-1---%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	                uncali_x_default[0], uncali_y_default[0],
	                uncali_x_default[1], uncali_y_default[1],
	                uncali_x_default[2], uncali_y_default[2],
	                uncali_x_default[3], uncali_y_default[3],
	                uncali_x_default[4], uncali_y_default[4]);
    	if (ret == 1){
    		ret = sprintf(_buf, "recovery\n");
    	}
    	else{
    		ret = sprintf(_buf, "fail\n");
   		}
#endif
    }
    
    //printk("Calibration status: _buf=<%s", _buf);
    
	return ret;
}
#ifdef TS_PRESSURE
static ssize_t touch_pressure(struct device *dev, struct device_attribute *attr, char *_buf)
{
	printk("enter %s gADPoint.x==%d,gADPoint.y==%d\n",__FUNCTION__,gADPoint.x,gADPoint.y);
	return sprintf(_buf,"%d,%d,%d\n",gZvalue[0],gZvalue[1],gZvalue[2]);
}
#endif

static struct class *tp_class = NULL;
static struct device *tp_dev = NULL;

static DEVICE_ATTR(touchcheck, 0666, touch_mode_show, touch_mode_store);
static DEVICE_ATTR(touchadc, 0666, touch_adc_show, NULL);
static DEVICE_ATTR(calistatus, 0666, touch_cali_status, NULL);
#ifdef TS_PRESSURE
static DEVICE_ATTR(pressure, 0666, touch_pressure, NULL);
#endif

static int __init tp_calib_iface_init(void)
{
    int ret = 0;
    int err = 0;
    
    tp_class = class_create(THIS_MODULE, "touchpanel");
    if (IS_ERR(tp_class)) 
    {
        printk("Create class touchpanel failed.\n");
        return -ENOMEM;
    }
/*
    memcpy(screen_x,x,5*sizeof(int));
	memcpy(screen_y,y,5*sizeof(int));
	memcpy(uncali_x_default,uncali_x,5*sizeof(int));
	memcpy(uncali_y_default,uncali_y,5*sizeof(int));
*/
    err = TouchPanelSetCalibration(4, screen_x, screen_y, uncali_x_default, uncali_y_default);
    	printk("tp_calib_iface_init---%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
	                uncali_x_default[0], uncali_y_default[0],
	                uncali_x_default[1], uncali_y_default[1],
	                uncali_x_default[2], uncali_y_default[2],
	                uncali_x_default[3], uncali_y_default[3],
	                uncali_x_default[4], uncali_y_default[4]);
  	if (err == 1){
		printk("Auto set calibration successfully.\n");
	} else {
		printk("Auto set calibraion failed, reset data again please !");
	}
    
    /*
	 * Create ifaces for TP calibration.
	 */
	tp_dev = device_create(tp_class, NULL, MKDEV(0, 1), NULL, "tp");
    ret =  device_create_file(tp_dev, &dev_attr_touchcheck);
    ret += device_create_file(tp_dev, &dev_attr_touchadc);
    ret += device_create_file(tp_dev, &dev_attr_calistatus);
#ifdef TS_PRESSURE
   ret += device_create_file(tp_dev, &dev_attr_pressure);
#endif
    if (ret)
    {
        printk("Fail to class ifaces for TP calibration.\n");
    }

    return ret;
}

static void __exit tp_calib_iface_exit(void)
{
    device_remove_file(tp_dev, &dev_attr_touchcheck);
    device_remove_file(tp_dev, &dev_attr_touchadc);
    device_remove_file(tp_dev, &dev_attr_calistatus);
#ifdef TS_PRESSURE
    device_remove_file(tp_dev, &dev_attr_pressure);
#endif
    device_destroy(tp_class, tp_dev->devt);
    class_destroy(tp_class);
}

module_init(tp_calib_iface_init);
module_exit(tp_calib_iface_exit);

MODULE_AUTHOR("Yongle Lai");
MODULE_DESCRIPTION("XPT2046 TPC driver @ Rockchip");
MODULE_LICENSE("GPL");
