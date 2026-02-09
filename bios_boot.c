/**
 * @file bios_boot.c
 * @brief Industrial Computer BIOS Boot Logic
 * @version 1.0.0
 * @hardware Mainboard v2.1 with Enhanced Power Supply
 * 
 * 고온 환경(공장)에서 안정적 부팅을 위한 BIOS 로직
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ========== 설정 상수 ==========
#define TEMP_NORMAL_MAX      85   // 일반 최대 온도 (°C)
#define TEMP_HIGH_MAX        300  // 고온 최대 온도 (°C) - 수정 필요
#define TEMP_CRITICAL        140  // 임계 온도
#define BOOT_RETRY_MAX       3    // 최대 재시도 횟수

// 부팅 상태 코드
#define BOOT_OK              0
#define BOOT_TEMP_WARNING    1
#define BOOT_TEMP_CRITICAL   2
#define BOOT_FAILED          3

// ========== 전역 변수 ==========
typedef struct {
    uint16_t cpu_temp;
    uint16_t board_temp;
    uint8_t boot_count;
    bool high_temp_mode;
} SystemState;

static SystemState g_state = {0};

// ========== 온도 읽기 함수 (하드웨어 시뮬레이션) ==========
uint16_t read_cpu_temperature(void) {
    // 실제로는 센서에서 읽어옴
    return g_state.cpu_temp;
}

uint16_t read_board_temperature(void) {
    return g_state.board_temp;
}

// ========== 로그 함수 ==========
void log_boot(const char* msg) {
    printf("[BOOT LOG] %s\n", msg);
}

void log_error(const char* msg) {
    printf("[ERROR] %s\n", msg);
}

// ========== 핵심 부팅 로직 ==========

/**
 * ISSUE: 고온 환경에서 부팅 실패 문제
 * 
 * 현재 문제:
 * - 공장 환경(고온)에서 온도가 85°C를 초과하면 부팅이 차단됨
 * - 하드웨어팀이 전원부를 강화했지만, BIOS 로직이 이를 반영하지 못함
 * 
 * 수정 방향:
 * 1. 고온 모드 감지 로직 추가
 * 2. 온도 임계값을 환경에 따라 동적 조정
 * 3. 고온에서도 안전하게 부팅하되, 모니터링 강화
 */
uint8_t check_thermal_conditions(void) {
    g_state.cpu_temp = read_cpu_temperature();
    g_state.board_temp = read_board_temperature();
    
    printf("CPU Temp: %d°C, Board Temp: %d°C\n", 
           g_state.cpu_temp, g_state.board_temp);
    
    // ===== 수정 전 로직 (문제 발생 지점) =====
    // 단순히 85°C만 체크 → 고온 환경에서 부팅 실패
    if (g_state.cpu_temp > TEMP_NORMAL_MAX) {
        log_error("Temperature too high for boot!");
        return BOOT_TEMP_CRITICAL;
    }
    
    // ===== 여기에 수정 로직 추가 예정 =====
    // TODO: 고온 모드 감지 및 동적 임계값 적용
    // TODO: 하드웨어 v2.1 전원부 강화 반영
    
    return BOOT_OK;
}

/**
 * 전원 안정성 체크
 * 하드웨어팀의 전원부 강화로 고온에서도 안정적 전원 공급 가능
 */
bool check_power_stability(void) {
    log_boot("Checking power supply stability...");
    
    // 전원부 v2.1 - 고온 대응 강화
    // 실제로는 전압/전류 센서 값 확인
    bool power_ok = true;
    
    if (power_ok) {
        log_boot("Power supply: STABLE");
        return true;
    } else {
        log_error("Power supply unstable!");
        return false;
    }
}

/**
 * 메인 부팅 시퀀스
 */
uint8_t bios_boot_sequence(void) {
    g_state.boot_count++;
    
    printf("\n========================================\n");
    printf("BIOS Boot Sequence v1.0.0\n");
    printf("Hardware: Mainboard v2.1\n");
    printf("Boot Attempt: %d\n", g_state.boot_count);
    printf("========================================\n\n");
    
    // Step 1: 전원 체크
    log_boot("Step 1: Power check");
    if (!check_power_stability()) {
        return BOOT_FAILED;
    }
    
    // Step 2: 온도 체크 (핵심 수정 지점)
    log_boot("Step 2: Thermal check");
    uint8_t thermal_status = check_thermal_conditions();
    if (thermal_status == BOOT_TEMP_CRITICAL) {
        log_error("Boot aborted due to thermal conditions");
        return BOOT_TEMP_CRITICAL;
    }
    
    // Step 3: 하드웨어 초기화
    log_boot("Step 3: Hardware initialization");
    log_boot("Initializing CPU...");
    log_boot("Initializing Memory...");
    log_boot("Initializing I/O...");
    
    // Step 4: 부팅 완료
    log_boot("Step 4: Boot complete");
    printf("\n[SUCCESS] System boot completed!\n\n");
    
    return BOOT_OK;
}

// ========== 테스트용 메인 함수 ==========
int main(void) {
    // 시나리오 1: 정상 온도 (실온 환경)
    printf("=== Test 1: Normal Temperature ===\n");
    g_state.cpu_temp = 75;
    g_state.board_temp = 70;
    bios_boot_sequence();
    
    // 시나리오 2: 고온 환경 (공장 - 현재 실패)
    printf("\n=== Test 2: High Temperature (Factory) ===\n");
    g_state.cpu_temp = 95;
    g_state.board_temp = 90;
    g_state.boot_count = 0;
    bios_boot_sequence();
    
    return 0;
}
