/*
 * Samsung Exynos5 SoC series FIMC-IS driver
 *
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef fimc_is_device_sensor_peri_H
#define fimc_is_device_sensor_peri_H

#ifdef CONFIG_MUIC_NOTIFIER
#include <linux/device.h>
#include <linux/muic/muic.h>
#include <linux/muic/muic_notifier.h>
#endif

#include <linux/interrupt.h>
#include "fimc-is-mem.h"
#include "fimc-is-param.h"
#include "fimc-is-interface-sensor.h"
#ifdef CONFIG_COMPANION_DIRECT_USE
#include "fimc-is-interface-preprocessor.h"
#endif
#include "fimc-is-control-sensor.h"

#define HRTIMER_IMPOSSIBLE		0
#define HRTIMER_POSSIBLE		1
#define VIRTUAL_COORDINATE_WIDTH		32768
#define VIRTUAL_COORDINATE_HEIGHT		32768
#define FIMC_IS_CIS_REV_MAX_LIST		3
#define VENDOR_SOFT_LANDING_STEP_MAX	2

#ifdef USE_CAMERA_ACT_DRIVER_SOFT_LANDING
enum HW_SOFTLANDING_STATE {
	HW_SOFTLANDING_PASS = 0,
	HW_SOFTLANDING_FAIL = -200,
};
#endif

struct fimc_is_cis {
	u32				id;
	struct v4l2_subdev		*subdev; /* connected module subdevice */
	u32				device; /* connected sensor device */
	struct i2c_client		*client;

	cis_shared_data			*cis_data;
	struct fimc_is_cis_ops		*cis_ops;
	enum otf_input_order		bayer_order;
	u32				aperture_num;
	bool				use_dgain;
	bool				hdr_ctrl_by_again;
	bool				use_wb_gain;
	bool				use_3hdr;

	struct fimc_is_sensor_ctl	sensor_ctls[CAM2P0_UCTL_LIST_SIZE];

	/* store current ctrl */
	camera2_sensor_uctl_t		cur_sensor_uctrl;

	/* settings for mode change */
	bool				need_mode_change;
	u32				mode_chg_expo;
	u32				mode_chg_again;
	u32				mode_chg_dgain;
	u32				mode_chg_long_expo;
	u32				mode_chg_long_again;
	u32				mode_chg_long_dgain;
	struct wb_gains			mode_chg_wb_gains;

	/* expected dms */
	camera2_lens_dm_t		expecting_lens_dm[EXPECT_DM_NUM];
	camera2_sensor_dm_t		expecting_sensor_dm[EXPECT_DM_NUM];
	camera2_flash_dm_t		expecting_flash_dm[EXPECT_DM_NUM];

	/* expected udm */
	camera2_lens_udm_t		expecting_lens_udm[EXPECT_DM_NUM];
	camera2_sensor_udm_t		expecting_sensor_udm[EXPECT_DM_NUM];

	/* For sensor status dump */
	struct work_struct		cis_status_dump_work;

	/* one more check_rev in mode_change */
	bool				rev_flag;

	/* For sensor revision checking */
	u32 				rev_addr;
	u32 				rev_byte;
	u32 				rev_valid_count;
	u32 				rev_valid_values[FIMC_IS_CIS_REV_MAX_LIST];

	/* get a min, max fps to HAL */
	u32				min_fps;
	u32				max_fps;
	struct mutex			*i2c_lock;
	struct mutex			control_lock;
	bool				use_pdaf;

	/* Long Term Exposure Mode(LTE mode) structure */
	struct fimc_is_long_term_expo_mode	long_term_mode;
	struct work_struct			long_term_mode_work;

	/* sensor control delay(N+1 or N+2) */
	u32				ctrl_delay;
#ifdef USE_CAMERA_MIPI_CLOCK_VARIATION
#ifdef USE_CAMERA_MIPI_CLOCK_VARIATION_RUNTIME
	struct work_struct				mipi_clock_change_work;
#endif
	int				mipi_clock_index_new;
	int				mipi_clock_index_cur;
#endif
	u32				ae_exposure;
	u32				ae_deltaev;
#ifdef USE_CAMERA_FACTORY_DRAM_TEST
	struct work_struct				factory_dramtest_work;
	u32				factory_dramtest_section2_fcount;
#endif

	/* settings for initial AE */
	bool				use_initial_ae;
	ae_setting			init_ae_setting;
	ae_setting			last_ae_setting;
};

struct fimc_is_actuator_data {
	struct timer_list		timer_wait;
	u32				check_time_out;

	bool				actuator_init;

	/* M2M AF */
	struct hrtimer              	afwindow_timer;
	struct work_struct		actuator_work;
	u32				timer_check;
};

struct fimc_is_actuator {
	u32					id;
	struct v4l2_subdev			*subdev; /* connected module subdevice */
	u32					device; /* connected sensor device */
	struct i2c_client			*client;

	u32					position;
	u32					max_position;

	/* for M2M AF */
	struct timeval				start_time;
	struct timeval				end_time;
	u32					valid_flag;
	ulong					valid_time;

	/* softlanding */
	bool                             need_softlanding;

	/* WinAf value for M2M AF */
	u32					left_x;
	u32 					left_y;
	u32 					right_x;
	u32 					right_y;

	int					actuator_index;

	u32					pre_position[EXPECT_DM_NUM];
	u32					pre_frame_cnt[EXPECT_DM_NUM];

	enum fimc_is_actuator_pos_size_bit	pos_size_bit;
	enum fimc_is_actuator_direction		pos_direction;

	struct fimc_is_actuator_data		actuator_data;
	struct fimc_is_device_sensor_peri	*sensor_peri;
	struct fimc_is_actuator_ops			*actuator_ops;
	struct mutex				*i2c_lock;

	u32				vendor_product_id;
	u32				vendor_first_pos;
	u32				vendor_first_delay;
	u32				vendor_soft_landing_list[VENDOR_SOFT_LANDING_STEP_MAX * 2];
	u32				vendor_soft_landing_list_len;
	bool				vendor_use_sleep_mode;
	bool				vendor_use_standby_mode;
	bool				vendor_use_update_pid;
};

struct fimc_is_aperture {
	u32				id;
	struct v4l2_subdev		*subdev; /* connected module subdevice */
	u32				device; /* connected sensor device */
	struct i2c_client		*client;
	struct fimc_is_aperture_ops		*aperture_ops;
	struct fimc_is_device_sensor_peri	*sensor_peri;
	struct mutex				*i2c_lock;
	struct mutex			control_lock;
	int				new_value;
	int				cur_value; /* need to mode when aperture value change */
	int				start_value;
	enum fimc_is_aperture_control_step	step;
	struct work_struct		aperture_set_start_work;
	struct work_struct		aperture_set_work;
};

struct fimc_is_flash_data {
	enum flash_mode			mode;
	u32				intensity;
	u32				firing_time_us;
	bool				flash_fired;
	struct work_struct		flash_fire_work;
	struct timer_list		flash_expire_timer;
	struct work_struct		flash_expire_work;
	struct work_struct		work_flash_muic_ctrl_and_fire;
};

struct fimc_is_flash {
	u32				id;
	struct v4l2_subdev		*subdev; /* connected module subdevice */
	u32				device; /* connected sensor device */
	struct i2c_client		*client;

	int				flash_gpio;
	int				torch_gpio;

	struct fimc_is_flash_data	flash_data;
	struct fimc_is_flash_expo_gain  flash_ae;

#ifdef CONFIG_CAMERA_FLASH_I2C_OBJ
	struct notifier_block		flash_noti_ta;
	int				attach_ta;
	int				attach_sdp;
#endif
	struct fimc_is_device_sensor_peri	*sensor_peri;
};

struct fimc_is_ois {
	u32				id;
	u32				device; /* connected sensor device */
	u32				ois_mode; /* need to mode when ois mode change */
	u32				pre_ois_mode; /* need to mode when ois mode change */
	bool				ois_shift_available;
	bool				ois_shift_available_rear2; /* need to mode when ois mode change */
	struct v4l2_subdev		*subdev; /* connected module subdevice */
	struct i2c_client		*client;

	struct fimc_is_ois_ops		*ois_ops;
	struct fimc_is_device_sensor_peri	*sensor_peri;
	struct mutex				*i2c_lock;
	u8				coef;
	u8				pre_coef;
	bool				fadeupdown;
	bool				initial_centering_mode;
#ifdef CAMERA_REAR2_OIS
	int				ois_power_mode;
#endif
	struct work_struct		ois_set_init_work;
};

struct fimc_is_mcu {
	struct v4l2_subdev			*subdev; /* connected module subdevice */
	struct i2c_client 			*client;
	u32						id;
	u32						device; /* connected sensor device */
	u32						ver;
	u32						name;
	int						gpio_mcu_reset;
	int						gpio_mcu_boot0;
	u8						vdrinfo_bin[4];
	u8						hw_bin[4];
	u8						vdrinfo_mcu[4];
	u8						hw_mcu[4];
	char						load_fw_name[50];
	struct fimc_is_ois			*ois;
	struct v4l2_subdev			*subdev_ois;
	struct fimc_is_device_ois	*ois_device;
	struct fimc_is_aperture		*aperture;
	struct v4l2_subdev		*subdev_aperture;
	struct fimc_is_aperture_ops		*aperture_ops;
	struct fimc_is_device_sensor_peri	*sensor_peri;
	struct mutex				*i2c_lock;
	void						*private_data;
};

struct fimc_is_preprocessor {
	u32				id;
	struct v4l2_subdev		*subdev; /* connected module subdevice */
	u32				device; /* connected sensor device */
	struct i2c_client		*client;
	struct fimc_is_device_preproc	*device_preproc;

	u32				cfgs;
	struct fimc_is_preproc_cfg	*cfg;

	struct fimc_is_preprocessor_ops	*preprocessor_ops;
#if defined(CONFIG_R7_VENDER_MCD)
	void				*private_data;
#endif
	struct spi_device		*spi;
	struct mutex			*i2c_lock;
};

struct fimc_is_pdp {
	u32				id;
	u32 __iomem			*base_reg;
	resource_size_t			regs_start;
	resource_size_t			regs_end;
	int				irq;
	size_t				width;
	size_t				height;
	struct mutex			control_lock;
	struct fimc_is_pdp_ops		*pdp_ops;
	struct v4l2_subdev		*subdev; /* connected module subdevice */
};

struct fimc_is_device_sensor_peri {
	struct fimc_is_module_enum	*module;

	struct fimc_is_cis		cis;
	struct v4l2_subdev		*subdev_cis;

	struct fimc_is_actuator		*actuator;
	struct v4l2_subdev		*subdev_actuator;

	struct fimc_is_flash		*flash;
	struct v4l2_subdev		*subdev_flash;

	struct fimc_is_preprocessor	*preprocessor;
	struct v4l2_subdev		*subdev_preprocessor;

	struct fimc_is_ois		*ois;
	struct v4l2_subdev		*subdev_ois;

	struct fimc_is_pdp		*pdp;
	struct v4l2_subdev		*subdev_pdp;

	struct fimc_is_aperture		*aperture;
	struct v4l2_subdev		*subdev_aperture;

	struct fimc_is_mcu		*mcu;
	struct v4l2_subdev		*subdev_mcu;

	unsigned long			peri_state;

	/* Thread for sensor and high spped recording setting */
	u32				sensor_work_index;
	spinlock_t			sensor_work_lock;
	struct task_struct		*sensor_task;
	struct kthread_worker		sensor_worker;
	struct kthread_work		sensor_work;

	/* Thread for sensor register setting */
	struct task_struct		*mode_change_task;
	struct kthread_worker		mode_change_worker;
	struct kthread_work		mode_change_work;

	/* first sensor mode setting flag */
        u32                             mode_change_first;

	struct fimc_is_sensor_interface		sensor_interface;
#ifdef CONFIG_COMPANION_DIRECT_USE
	struct fimc_is_preprocessor_interface	preprocessor_inferface;
#endif
	int						reuse_3a_value;
};

void fimc_is_sensor_work_fn(struct kthread_work *work);
void fimc_is_sensor_mode_change_work_fn(struct kthread_work *work);
int fimc_is_sensor_init_sensor_thread(struct fimc_is_device_sensor_peri *sensor_peri);
void fimc_is_sensor_deinit_sensor_thread(struct fimc_is_device_sensor_peri *sensor_peri);
int fimc_is_sensor_init_mode_change_thread(struct fimc_is_device_sensor_peri *sensor_peri);
void fimc_is_sensor_deinit_mode_change_thread(struct fimc_is_device_sensor_peri *sensor_peri);

struct fimc_is_device_sensor_peri *find_peri_by_cis_id(struct fimc_is_device_sensor *device,
							u32 cis);
struct fimc_is_device_sensor_peri *find_peri_by_act_id(struct fimc_is_device_sensor *device,
							u32 actuator);
struct fimc_is_device_sensor_peri *find_peri_by_flash_id(struct fimc_is_device_sensor *device,
							u32 flash);
struct fimc_is_device_sensor_peri *find_peri_by_preprocessor_id(struct fimc_is_device_sensor *device,
							u32 preprocessor);
struct fimc_is_device_sensor_peri *find_peri_by_ois_id(struct fimc_is_device_sensor *device,
							u32 ois);

void fimc_is_sensor_set_cis_uctrl_list(struct fimc_is_device_sensor_peri *sensor_peri,
		u32 long_exp, u32 short_exp,
		u32 long_total_gain, u32 short_total_gain,
		u32 long_analog_gain, u32 short_analog_gain,
		u32 long_digital_gain, u32 short_digital_gain);

int fimc_is_sensor_peri_notify_vsync(struct v4l2_subdev *subdev, void *arg);
int fimc_is_sensor_peri_notify_vblank(struct v4l2_subdev *subdev, void *arg);
int fimc_is_sensor_peri_notify_flash_fire(struct v4l2_subdev *subdev, void *arg);
int fimc_is_sensor_peri_pre_flash_fire(struct v4l2_subdev *subdev, void *arg);
int fimc_is_sensor_peri_notify_actuator(struct v4l2_subdev *subdev, void *arg);
int fimc_is_sensor_peri_notify_m2m_actuator(void *arg);
int fimc_is_sensor_peri_notify_actuator_init(struct v4l2_subdev *subdev);

int fimc_is_sensor_peri_call_m2m_actuator(struct fimc_is_device_sensor *device);
int fimc_is_sensor_initial_preprocessor_setting(struct fimc_is_device_sensor_peri *sensor_peri);

enum hrtimer_restart fimc_is_actuator_m2m_af_set(struct hrtimer *timer);

int fimc_is_actuator_notify_m2m_actuator(struct v4l2_subdev *subdev);

void fimc_is_sensor_peri_probe(struct fimc_is_device_sensor_peri *sensor_peri);
int fimc_is_sensor_peri_s_stream(struct fimc_is_device_sensor *device,
					bool on);

int fimc_is_sensor_peri_s_frame_duration(struct fimc_is_device_sensor *device,
				u32 frame_duration);
int fimc_is_sensor_peri_s_exposure_time(struct fimc_is_device_sensor *device,
				u32 long_exposure_time, u32 short_exposure_time);
int fimc_is_sensor_peri_s_analog_gain(struct fimc_is_device_sensor *device,
				u32 long_analog_gain, u32 short_analog_gain);
int fimc_is_sensor_peri_s_digital_gain(struct fimc_is_device_sensor *device,
				u32 long_digital_gain, u32 short_digital_gain);
int fimc_is_sensor_peri_s_wb_gains(struct fimc_is_device_sensor *device,
				struct wb_gains wb_gains);
int fimc_is_sensor_peri_adj_ctrl(struct fimc_is_device_sensor *device,
				u32 input, struct v4l2_control *ctrl);
int fimc_is_sensor_set_test_pattern(struct fimc_is_device_sensor *device,
				camera2_sensor_ctl_t *sensor_ctl);
int fimc_is_sensor_peri_compensate_gain_for_ext_br(struct fimc_is_device_sensor *device,
				u32 expo, u32 *again, u32 *dgain);

int fimc_is_sensor_peri_actuator_softlanding(struct fimc_is_device_sensor_peri *device);

int fimc_is_sensor_peri_debug_fixed(struct fimc_is_device_sensor *device);

void fimc_is_sensor_flash_fire_work(struct work_struct *data);
void fimc_is_sensor_flash_expire_handler(unsigned long data);
void fimc_is_sensor_flash_expire_work(struct work_struct *data);
int fimc_is_sensor_flash_fire(struct fimc_is_device_sensor_peri *device,
				u32 on);

void fimc_is_sensor_actuator_soft_move(struct work_struct *data);

void fimc_is_sensor_long_term_mode_set_work(struct work_struct *data);

int fimc_is_sensor_mode_change(struct fimc_is_cis *cis, u32 mode);
void fimc_is_sensor_peri_init_work(struct fimc_is_device_sensor_peri *sensor_peri);

#define CALL_CISOPS(s, op, args...) (((s)->cis_ops->op) ? ((s)->cis_ops->op(args)) : 0)
#define CALL_PREPROPOPS(s, op, args...) (((s)->preprocessor_ops->op) ? ((s)->preprocessor_ops->op(args)) : 0)
#define CALL_OISOPS(s, op, args...) (((s)->ois_ops->op) ? ((s)->ois_ops->op(args)) : 0)
#define CALL_ACTUATOROPS(s, op, args...) (((s)->actuator_ops->op) ? ((s)->actuator_ops->op(args)) : 0)
#define CALL_APERTUREOPS(s, op, args...) (((s)->aperture_ops->op) ? ((s)->aperture_ops->op(args)) : 0)
#define CALL_PDPOPS(s, op, args...) (((s)->pdp_ops->op) ? ((s)->pdp_ops->op(args)) : 0)
#endif
