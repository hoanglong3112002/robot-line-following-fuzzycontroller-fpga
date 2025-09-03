#include <stdio.h>
#include "system.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_uart_regs.h"
#include "sys/alt_irq.h"
#include "io.h"
#include <unistd.h> // Thư viện cho usleep

void Timer_Init(void) {
    unsigned int period = 0;

    // Dừng Timer
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE, ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);

    // Thiết lập chu kỳ của Timer thành 1 giây
    period = 50000000 - 1;  // Clock của Timer là 50 MHz
    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, period);
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, (period >> 16));

    // Cấu hình và Bắt đầu Timer
    IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_0_BASE,
                                     ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
                                     ALTERA_AVALON_TIMER_CONTROL_ITO_MSK |
                                     ALTERA_AVALON_TIMER_CONTROL_START_MSK);
}

int count1 = 0, count2 = 0, second = 0;
unsigned int encoder1_data, encoder2_data, prev_encoder1_data, prev_encoder2_data, rpm1, rpm2, rpm_do = 0;
unsigned int sensor_data;
int rpm_dat = 150;
int time_high = 3000;
int time_low = 2000;

void demxung(void) {
    encoder1_data = IORD_ALTERA_AVALON_PIO_DATA(ENCODER1_BASE);
    encoder2_data = IORD_ALTERA_AVALON_PIO_DATA(ENCODER2_BASE);
    if (encoder1_data != prev_encoder1_data && encoder1_data == 1) {
        count1++;
        rpm1 = (count1 / 20.0) * 60.0;
    }
    if (encoder2_data != prev_encoder2_data && encoder2_data == 1) {
        count2++;
        rpm2 = (count2 / 20.0) * 60.0;
    }
    rpm_do = (rpm1 + rpm2) / 2;
    prev_encoder1_data = encoder1_data;
    prev_encoder2_data = encoder2_data;
}

void queotrai() {
    IOWR_ALTERA_AVALON_PIO_DATA(IN2_BASE, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(IN3_BASE, 0);
    usleep(time_high);
    IOWR_ALTERA_AVALON_PIO_DATA(IN2_BASE, 0);
    usleep(time_low);
}

void queotraigap() {
    IOWR_ALTERA_AVALON_PIO_DATA(IN2_BASE, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(IN4_BASE, 1);
    usleep(time_high);
    IOWR_ALTERA_AVALON_PIO_DATA(IN2_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(IN4_BASE, 0);
    usleep(time_low);
}

void dithang() {
    IOWR_ALTERA_AVALON_PIO_DATA(IN2_BASE, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(IN3_BASE, 1);
    usleep(time_high);
    IOWR_ALTERA_AVALON_PIO_DATA(IN2_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(IN3_BASE, 0);
    usleep(time_low);
}

void queophai() {
    IOWR_ALTERA_AVALON_PIO_DATA(IN3_BASE, 1);
    usleep(time_high);
    IOWR_ALTERA_AVALON_PIO_DATA(IN3_BASE, 0);
    usleep(time_low);
}

void queophaigap() {
    IOWR_ALTERA_AVALON_PIO_DATA(IN3_BASE, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(IN1_BASE, 1);
    usleep(time_high);
    IOWR_ALTERA_AVALON_PIO_DATA(IN3_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(IN1_BASE, 0);
    usleep(time_low);
}

void dilui() {
    IOWR_ALTERA_AVALON_PIO_DATA(IN1_BASE, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(IN4_BASE, 1);
    usleep(time_high);
    IOWR_ALTERA_AVALON_PIO_DATA(IN1_BASE, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(IN4_BASE, 0);
    usleep(time_low);
}

float get_sensor_value(int sensor_data) {
	switch (sensor_data) {
		case 11: return 0.0526;
		case 5:  return 0.0588;
		case 18: return 0.1176;
		case 1:  return 0.1764;
		case 3:  return 0.2352;
		case 7:  return 0.2941;
		case 15: return 0.3529;
		case 23: return 0.4117;
		case 19: return 0.4705;
		case 17: return 0.5294; // SP
		case 27: return 0.5294; // SP
		case 25: return 0.5882;
		case 29: return 0.6470;
		case 30: return 0.7058;
		case 28: return 0.7647;
		case 24: return 0.8235;
		case 16: return 0.8823;
		case 20: return 0.9411;
		case 26: return 0.9473;
		case 13: return 1;
		default: return 0.0; // Giá trị mặc định cho các trường hợp khác
	}
}


void calculate_error_and_delta(int sensor_data, float *error, float *delta_error, float *previous_error) {
	float current_value = get_sensor_value(sensor_data);
	*error = current_value - 0.5294; //tru cho gia tri diem chuan la 0.529
	*delta_error = *error - *previous_error;
	*previous_error = *error;
}

float fuzzy_triangle_min(float x, float a, float b) {
    if (x >= a && x < b) {
        return (b - x) / (b - a);
    } else {
        return 0;
    }
}

float fuzzy_length_max(float x, float a, float b) {
    if (x > a && x < b) {
        return (x - a) / (b - a);
    } else if (x >= b) {
        return 1;
    } else {
        return 0;
    }
}

float fuzzy_triangle(float x, float a, float b, float c) {
    if (x <= a || x >= c) {
        return 0;
    } else if (x > a && x <= b) {
        return (x - a) / (b - a);
    } else if (x > b && x < c) {
        return (c - x) / (c - b);
    } else {
        return 0;
    }
}

float error_Sieuam(const float *error) {
    float a = -0.4737f, b = -0.3158f;
    return fuzzy_triangle_min(*error, a, b);
}

float error_Am(const float *error) {
    float a = -0.4737f, b = -0.3158f, c= 0.0f;
    return fuzzy_triangle(*error, a, b, c);
}

float error_Zero(const float *error) {
    float a = -0.4737f, b = 0.0f, c = 0.4737f;
    return fuzzy_triangle(*error, a, b, c);
}

float error_Duong(const float *error) {
    float a = 0.0f, b = 0.3158f, c = 0.4737f;
    return fuzzy_triangle(*error, a, b, c);
}

float error_Sieuduong(const float *error) {
    float a = 0.3158f, b = 0.4737f;
    return fuzzy_length_max(*error, a, b);
}

float delerror_Sieuam(const float *error) {
    float a = -0.4737f, b = -0.3158f;
    return fuzzy_triangle_min(*error, a, b);
}

float delerror_Am(const float *error) {
    float a = -0.4737f, b = -0.3158f, c= 0.0f;
    return fuzzy_triangle(*error, a, b, c);
}

float delerror_Zero(const float *error) {
    float a = -0.4737f, b = 0.0f, c = 0.4737f;
    return fuzzy_triangle(*error, a, b, c);
}

float delerror_Duong(const float *error) {
    float a = 0.0f, b = 0.3158f, c = 0.4737f;
    return fuzzy_triangle(*error, a, b, c);
}

float delerror_Sieuduong(const float *error) {
    float a = 0.3158f, b = 0.4737f;
    return fuzzy_length_max(*error, a, b);
}

// max-min
float fuzzy(float error, float delerror) {

    float error_sieuam = error_Sieuam(&error);
    float error_am = error_Am(&error);
    float error_zero = error_Zero(&error);
    float error_duong = error_Duong(&error);
    float error_sieuduong = error_Sieuduong(&error);

    float delerror_sieuam = delerror_Sieuam(&delerror);
    float delerror_am = delerror_Am(&delerror);
    float delerror_zero = delerror_Zero(&delerror);
    float delerror_duong = delerror_Duong(&delerror);
    float delerror_sieuduong = delerror_Sieuduong(&delerror);

    float min_SN_SN = fminf(error_sieuam, delerror_sieuam);
    float min_SN_N = fminf(error_sieuam, delerror_am);
    float min_SN_Z = fminf(error_sieuam, delerror_zero);
    float min_SN_P = fminf(error_sieuam, delerror_duong);
    float min_SN_SP = fminf(error_sieuam, delerror_sieuduong);

    float min_N_SN = fminf(error_am, delerror_sieuam);
    float min_N_N = fminf(error_am, delerror_am);
    float min_N_Z = fminf(error_am, delerror_zero);
    float min_N_P = fminf(error_am, delerror_duong);
    float min_N_SP = fminf(error_am, delerror_sieuduong);


    float min_Z_SN = fminf(error_zero, delerror_sieuam);
    float min_Z_N = fminf(error_zero, delerror_am);
    float min_Z_Z = fminf(error_zero, delerror_zero);
    float min_Z_P = fminf(error_zero, delerror_duong);
    float min_Z_SP = fminf(error_zero, delerror_sieuduong);


    float min_P_SN = fminf(error_duong, delerror_sieuam);
    float min_P_N = fminf(error_duong, delerror_am);
    float min_P_Z = fminf(error_duong, delerror_zero);
    float min_P_P = fminf(error_duong, delerror_duong);
    float min_P_SP = fminf(error_duong, delerror_sieuduong);


    float min_SP_SN = fminf(error_sieuduong, delerror_sieuam);
    float min_SP_N = fminf(error_sieuduong, delerror_am);
    float min_SP_Z = fminf(error_sieuduong, delerror_zero);
    float min_SP_P = fminf(error_sieuduong, delerror_duong);
    float min_SP_SP = fminf(error_sieuduong, delerror_sieuduong);

    float output_SL = fmaxf(fmaxf(fmaxf(fmaxf(fmaxf(fmaxf(min_SN_SN, min_SN_N), min_SN_Z), min_SN_P), min_SN_SP), min_N_SN), min_Z_SN);
    float output_L = fmaxf(fmaxf(fmaxf(min_N_N, min_N_Z), min_N_P), min_Z_N);
    float output_F = min_Z_Z;
    float output_R = fmaxf(fmaxf(fmaxf(min_P_N, min_P_Z), min_P_P), min_Z_P);
    float output_SR = fmaxf(fmaxf(fmaxf(fmaxf(fmaxf(fmaxf(min_SP_SN, min_SP_N), min_SP_Z), min_SP_P), min_SP_SP), min_P_SP), min_Z_SP);


    float Out = (output_SL * 0 + output_L * 0.25 + output_F * 0.5 + output_R * 0.75 + output_SR * 1) / (output_SL + output_L + output_F + output_R + output_SR);
    return Out;
}

void decide_direction(float fuzzy_output) {
    if (fuzzy_output >= 0.0 && fuzzy_output < 0.25) {
        queotraigap();
    } else if (fuzzy_output >= 0.25 && fuzzy_output < 0.5) {
        queotrai();
    } else if (fuzzy_output == 0.5) {
        dithang();
    } else if (fuzzy_output > 0.5 && fuzzy_output <= 0.75) {
        queophai();
    } else if (fuzzy_output > 0.75 && fuzzy_output <= 1.0) {
        queophaigap();
    } else {
        dilui();
    }
}


char number[10] = {'0','1','2','3','4','5','6','7','8','9'};
void uart_send_string(const char* str) {
    while (*str) {
        IOWR_ALTERA_AVALON_UART_TXDATA(UART_BASE, *str++);
        usleep(1000); // 1ms delay
    }
}

void Timer_IRQ_Handler(void* isr_context) {
    if (second == 1) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "V_do: %d D: 30%%\n", rpm_do);
        uart_send_string(buffer);

        second = 0;
        count1 = 0;
        count2 = 0;
    }
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, ALTERA_AVALON_TIMER_STATUS_TO_MSK);
}

int main() {
    Timer_Init();
    alt_ic_isr_register(0, TIMER_0_IRQ, Timer_IRQ_Handler, (void*)0, (void*)0);

    
    float error = 0.0, delta_error = 0.0, previous_error = 0.0;

    while (1) {
        if ((IORD(KEY1_BASE, 0) & 0x1) == 0) {
            while (1) {
                sensor_data = IORD_ALTERA_AVALON_PIO_DATA(SENSOR_BASE);

                calculate_error_and_delta(sensor_data, &error, &delta_error, &previous_error);

                float fuzzy_output = fuzzy(error, delta_error);

                decide_direction(fuzzy_output);

                demxung();
            }
        }
    }
    return 0;
}





