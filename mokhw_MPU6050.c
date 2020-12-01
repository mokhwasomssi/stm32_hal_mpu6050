#include "mokhw_MPU6050.h"


/*i2c통신, MPU6050 연결 확인*/
void WHO_AM_I(I2C_HandleTypeDef* hi2c)
{
	uint8_t slave_address = 0xD0; //0x68 왼쪽으로 한칸 쉬프팅
	uint8_t register_to_access = ADDRESS_WHO_AM_I; //접근할 레지스터(WHO_AM_I)의 주소
	uint8_t temp = 0; //WHO_AM_I에 저장된 값을 이 변수로 받음

	HAL_I2C_Master_Transmit(hi2c, slave_address, &register_to_access, 1, 1000); //읽으려는 레지스터의 주소값 보내고
	HAL_I2C_Master_Receive(hi2c, slave_address, &temp, 1, 1000); //WHO_AM_I에 저장되있는값(0x68) temp로 받음

	while (temp != 0x68); //받은 값이 0x68과 다르면 센서 잘못 연결
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); //받은 값이 0x68이라면 잘 연결, LED 켜짐
														//PA5는 STM32f103RBT6의 내장 LED
}

/*sleep mode -> waking up*/
void wake_up(I2C_HandleTypeDef* hi2c)
{
        uint8_t slave_address = 0xD0; //0x68 왼쪽으로 한칸 쉬프팅
        uint8_t data_to_write[2]; //레지스터 주소와 그 레지스터에 새로 넣어줄 값를 저장하는 배열

        data_to_write[0] = ADDRESS_PWR_MGMT_1; //접근할 레지스터(PWR_MGMT_1)의 주소
        data_to_write[1] = 0x00; //초기 값 0x40 -> 0x00으로 바꿔줌 : wake_up
        
        //배열을 이용해서 레지스터 주소와 해당되는 레지스터의 새로운 값를 한번에 보냄
        HAL_I2C_Master_Transmit(hi2c, slave_address, data_to_write, 2, 1000); 
        //해당 레지스터 값 바뀜
}


/*원하는 샘플 레이트 설정*/
void set_sample_rate(I2C_HandleTypeDef* hi2c, uint16_t sample_rate_you_want) 
{
	uint8_t slave_address = 0xD0; //0x68 왼쪽으로 한칸 쉬프팅
	uint8_t data_to_write[2];
        
	/*sample rate = gyro output rate / (1 + SMPLRT_DIV)*/
    /*원하는 샘플 레이트를 설정하기 위해, SMPLRT_DIV에 들어갈 값를 구하는 식*/
    uint8_t calculated_SMPLRT_DIV = (gyro_output_rate/sample_rate_you_want) - 1;
        
	data_to_write[0] = ADDRESS_SMPLRT_DIV; //접근할 레지스터(SMPLRT_DIV)의 주소
	data_to_write[1] = calculated_SMPLRT_DIV; //해당되는 레지스터의 새로운 값

    //배열을 이용해서 레지스터 주소와 해당되는 레지스터의 새로운 값를 한번에 보냄
	HAL_I2C_Master_Transmit(hi2c, slave_address, data_to_write, 2, 1000); 
    //레지스터 값 바뀜
}

/*자이로와 가속도의 측정 범위 설정*/
void set_sensitivity(I2C_HandleTypeDef* hi2c, mpu6050* __my_mpu6050, gyro_full_scale_range gyro_range_you_want, accel_full_scale_range accel_range_you_want)
{
	//자이로 엑셀로 민감도 정하기
	uint8_t slave_address = 0xD0; //0x68 왼쪽으로 한칸 쉬프팅
	uint8_t data_to_write[3];

	data_to_write[0] = ADDRESS_GYRO_CONFIG; //접근할 레지스터(GYRO_CONFIG)의 주소
	data_to_write[1] = gyro_range_you_want <<3; //해당되는 레지스터의 새로운 값
	data_to_write[2] = accel_range_you_want <<3; //해당되는 레지스터의 새로운 값

    //배열을 이용해서 레지스터 주소와 해당되는 레지스터의 새로운 값를 한번에 보냄
	HAL_I2C_Master_Transmit(hi2c, slave_address, data_to_write, 3, 1000);
    //레지스터 값 바뀜


	//자이로 측정 범위에 따라 gyro_change_unit_factor 값 바꿔주기
	switch (gyro_range_you_want) 
	{
	case gyro_full_scale_range_250 :
		__my_mpu6050->gyro_change_unit_factor = 131;
		break;

	case gyro_full_scale_range_500 :
		__my_mpu6050->gyro_change_unit_factor = 65.5;
		break;

	case gyro_full_scale_range_1000:
		__my_mpu6050->gyro_change_unit_factor = 32.8;
		break;

	case gyro_full_scale_range_2000:
		__my_mpu6050->gyro_change_unit_factor = 16.4;
		break;

	default :
		break;
	}

	//가속도 측정 범위에 따라 accel_change_unit_factor 값 바꿔주기
	switch (accel_range_you_want) 
	{
	case accel_full_scale_range_2g :
		__my_mpu6050->accel_change_unit_factor = 16384;
		break;

	case accel_full_scale_range_4g:
		__my_mpu6050->accel_change_unit_factor = 8192;
		break;

	case accel_full_scale_range_8g:
		__my_mpu6050->accel_change_unit_factor = 4096;
		break;

	case accel_full_scale_range_16g:
		__my_mpu6050->accel_change_unit_factor = 2048;
		break;

	default :
		break;
	}

}



/*자이로값 읽기*/
void read_gyro(I2C_HandleTypeDef* hi2c, mpu6050* __my_mpu6050, unit unit_you_want)
{
	uint8_t slave_address = 0xD0; //0x68 왼쪽으로 한칸 쉬프팅
	uint8_t register_to_access = ADDRESS_GYRO_XOUT_H; //접근할 레지스터(GYRO_XOUT_H)의 주소
	uint8_t data_to_read[6]; //자이로 값 저장하는 배열

	//읽을 레지스터 주소(GYRO_XOUT_H) 보내줌
	HAL_I2C_Master_Transmit(hi2c, slave_address, &register_to_access, 1, 1000); 

	//해당하는 레지스터의 데이터를 배열에 저장
	HAL_I2C_Master_Receive(hi2c, slave_address, data_to_read, 6, 1000);

	switch (unit_you_want) //센서 값을 원하는 단위로 바꿔줌
	{
	case raw_data : //바로 저장
		__my_mpu6050->gy_x = (int16_t)(data_to_read[0] << 8 | data_to_read[1]);
		__my_mpu6050->gy_y = (int16_t)(data_to_read[2] << 8 | data_to_read[3]);
		__my_mpu6050->gy_z = (int16_t)(data_to_read[4] << 8 | data_to_read[5]);
		break;

	case deg_per_sec : //단위 변환을 위해 raw data를 gyro_change_unit_factor로 나눈 값을 저장
		__my_mpu6050->gy_x_dps = (int16_t)(data_to_read[0] << 8 | data_to_read[1]) / __my_mpu6050->gyro_change_unit_factor;
		__my_mpu6050->gy_y_dps = (int16_t)(data_to_read[2] << 8 | data_to_read[3]) / __my_mpu6050->gyro_change_unit_factor;
		__my_mpu6050->gy_z_dps = (int16_t)(data_to_read[4] << 8 | data_to_read[5]) / __my_mpu6050->gyro_change_unit_factor;
		break;

	default :
		break;
	}
}


/*가속도 값 읽기*/
void read_accel(I2C_HandleTypeDef* hi2c, mpu6050* __my_mpu6050, unit unit_you_want)
{
	uint8_t slave_address = 0xD0; //0x68 왼쪽으로 한칸 쉬프팅
	uint8_t register_to_access = ADDRESS_ACCEL_XOUT_H;//접근할 레지스터(ACCEL_XOUT_H)의 주소
	uint8_t data_to_read[6]; //가속도 값 저장하는 배열

	 //읽을 레지스터 주소(ACCEL_XOUT_H) 보내줌
	HAL_I2C_Master_Transmit(hi2c, slave_address, &register_to_access, 1, 1000);

	//해당하는 레지스터의 데이터를 배열에 저장
	HAL_I2C_Master_Receive(hi2c, slave_address, data_to_read, 6, 1000);

    //16비트 센서 데이터가 8비트씩 나누어져 있어서
    //처음 받은 데이터를 왼쪽으로 8칸 쉬프팅하고 2번째로 받은 데이터를 합친다.

	switch (unit_you_want) //센서 값을 원하는 단위로 바꿔줌
	{
	case raw_data : //바로 저장
		__my_mpu6050->ac_x = data_to_read[0] << 8 | data_to_read[1];
		__my_mpu6050->ac_y = data_to_read[2] << 8 | data_to_read[3];
		__my_mpu6050->ac_z = data_to_read[4] << 8 | data_to_read[5];
		break;

	case gravity_acceleration : //단위 변환을 위해 raw data를 accel_change_unit_factor로 나눈 값을 저장
		__my_mpu6050->ac_x_g = (int16_t)(data_to_read[0] << 8 | data_to_read[1]) / __my_mpu6050->accel_change_unit_factor;
		__my_mpu6050->ac_y_g = (int16_t)(data_to_read[2] << 8 | data_to_read[3]) / __my_mpu6050->accel_change_unit_factor;
		__my_mpu6050->ac_z_g = (int16_t)(data_to_read[4] << 8 | data_to_read[5]) / __my_mpu6050->accel_change_unit_factor;
		break;

	default :
		break;
	}
}