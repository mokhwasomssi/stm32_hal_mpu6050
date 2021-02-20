# STM32_HAL_MPU6050_lib

## 1. 개요 
MPU6050를 쉽게 사용할 수 있도록 함수를 정의해놓은 라이브러리  
센서 값을 받기위해 필요한 최소한의 것만 포함했습니다.   
(인터럽트 없음, mpu6050 1개만 연결할때의 코드)  

## 사용 툴
- Iar Embedded Workbench IDE  
- STM32CubeMx (hal library)

## 3. 파일 목록
main.c : 라이브러리 사용 예시  
mokhw_MPU6050.h : 헤더 파일  
mokhw_MPU6050.c : 소스 파일  

## 4. 함수 목록


```
void WHO_AM_I(I2C_HandleTypeDef* hi2c)
```
### brief
MPU6050를 연결했는지 확인, 맞으면 엘이디 켜짐

### parameter
__hi2c__ : i2c 설정을 포함하는 I2C_HandleTypeDef 구조체 변수를 가리키는 포인터

---
```
void wake_up(I2C_HandleTypeDef* hi2c)
```
### brief
sleep mode인 센서를 깨워줌(?), sleep mode에서는 센서가 작동하지않음  
초기 값 0x40 : sleep mode -> 변경 값 0x00 : sleep mode 해제

### parameter
__hi2c__ : i2c 설정을 포함하는 I2C_HandleTypeDef 구조체 변수를 가리키는 포인터

---
### brief
ADC 샘플 레이트를 정하는 함수

### parameter
__hi2c__ : i2c 설정을 포함하는 I2C_HandleTypeDef 구조체 변수를 가리키는 포인터  
__sample_rate_you_want__ : 사용자가 원하는 샘플 레이트 4 ≤ sample_rate_you_want ≤ 8000 (단위 Hz)

---
```
﻿void set_sensitivity( I2C_HandleTypeDef* hi2c, 
                      mpu6050* __my_mpu6050, 
                      gyro_full_scale_range gyro_range_you_want, 
                      accel_full_scale_range accel_range_you_want )
```

## brief

자이로스코프와 가속도 센서의 측정 범위를 정하는 함수

## parameter

__hi2c__ : i2c 설정을 포함하는 I2C_HandleTypeDef 구조체 변수를 가리키는 포인터    
____my_mpu6050__ : mpu6050 구조체 변수를 가리키는 포인터  
__gyro_range_you_want__ : 원하는 자이로스코프 측정 범위  
__accel_range_you_want__ : 원하는 가속도 센서의 측정 범위  

---

```
void read_gyro( I2C_HandleTypeDef* hi2c, 
                mpu6050* __my_mpu6050, 
                unit unit_you_want )
```
## brief
i2c통신을 이용해 샘플링 된 각속도를 받아서 mpu6050 구조체 변수에 저장

## parameter

__hi2c__ : i2c 설정을 포함하는 I2C_HandleTypeDef 구조체 변수를 가리키는 포인터  
____my_mpu6050__ : mpu6050 구조체 변수를 가리키는 포인터  
__unit_you_want__ : 원하는 단위로 센서 값 변환 됨  

---

```
﻿void read_accel( I2C_HandleTypeDef* hi2c, 
                 mpu6050* __my_mpu6050, 
                 unit unit_you_want)
```

## brief
i2c통신을 이용해 샘플링 된 가속도를 받아서 mpu6050 구조체 변수에 저장

## parameter
__hi2c__ : i2c 설정을 포함하는 I2C_HandleTypeDef 구조체 변수를 가리키는 포인터  
____my_mpu6050__ : mpu6050 구조체 변수를 가리키는 포인터  
__unit_you_want__ : 원하는 단위로 센서 값 변환 됨  
