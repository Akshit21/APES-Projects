#ifndef _TMP1021_H__
#define _TMP1021_H__

#define TMP1021_ADDR  (0x48)

#define TEMP_REG  (0x00)
#define CONFIG_REG (0x01)
#define THRESH_REGL  (0x02)
#define THRESH_REGH (0x03)

#define CONFIG_SD_MASK                  (0x0001)
#define CONFIG_SD_ENABLED               (0x0001)
#define CONFIG_SD_DISABLED              (0x0000)
#define CONFIG_TM_MASK                  (0x0002)
#define CONFIG_TM_COMPARATOR_MODE       (0x0000)
#define CONFIG_TM_INTERRUPT_MODE        (0x0002)
#define CONFIG_POL_MASK                 (0x0004)
#define CONFIG_POL_ACTIVE_HIGH          (0x0004)
#define CONFIG_POL_ACTIVE_LOW           (0x0000)
#define CONFIG_FQ_MASK                  (0x0018)
#define CONFIG_FQ_CONSECUTIVE_FAULTS_1  (0x0000)
#define CONFIG_FQ_CONSECUTIVE_FAULTS_2  (0x0008)
#define CONFIG_FQ_CONSECUTIVE_FAULTS_3  (0x0010)
#define CONFIG_FQ_CONSECUTIVE_FAULTS_4  (0x0018)
#define CONFIG_CR_READ_MASK             (0x0060)
#define CONFIG_OS_MASK                  (0x0080)
#define CONFIG_OS_ONESHOT               (0x0080)
#define CONFIG_EM_MASK                  (0x1000)
#define CONFIG_EM_NORMAL_MODE           (0x0000)
#define CONFIG_EM_EXTENDED_MODE         (0x1000)
#define CONFIG_AL_MASK                  (0x2000)
#define CONFIG_CR_MASK                  (0xC000)
#define CONFIG_CR_0_25HZ                (0x0000)
#define CONFIG_CR_1HZ                   (0x4000)
#define CONFIG_CR_4HZ                   (0x8000)
#define CONFIG_CR_8HZ                   (0xC000)

#define CONFIG_DEFAULT  CONFIG_SD_DISABLED | CONFIG_TM_COMPARATOR_MODE \
                         CONFIG_POL_ACTIVE_HIGH | CONFIG_FQ_CONSECUTIVE_FAULTS_1 \
                         CONFIG_EM_NORMAL_MODE | CONFIG_CR_4HZ

#define CELSIUS_FORMAT  (0x00)
#define FAHRENHEIT_FORMAT (0x01)

#endif
