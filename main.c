#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "uart.h"

SemaphoreHandle_t xResourceMutex;

void vApplicationIdleHook(void) {
}

/* Tâche 1 : Faible Priorité (Priorité 1) */
void vTaskLow(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(50)); // Attente initiale pour stabiliser l'affichage

    for (;;) {
        UART_SendString("[LOW] Demande Mutex...\r\n");

        if (xSemaphoreTake(xResourceMutex, portMAX_DELAY) == pdTRUE) {
            UART_SendString("[LOW] Mutex Acquis ! Debut travail...\r\n");

            /* Charge CPU moyenne pour ne pas faire planter la pile */
            for (volatile uint16_t i = 0; i < 25000; i++) {
                asm volatile ("nop");
            }

            UART_SendString("[LOW] Travail Termine. Liberation Mutex...\r\n");
            xSemaphoreGive(xResourceMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1200));
    }
}

/* Tâche 2 : Priorité Moyenne (Priorité 2) */
void vTaskMedium(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(200));

    for (;;) {
        UART_SendString("  [MEDIUM] Tache Moyenne tourne\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* Tâche 3 : Haute Priorité (Priorité 3) */
void vTaskHigh(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(350));

    for (;;) {
        UART_SendString("    [HIGH] Demande Mutex...\r\n");

        if (xSemaphoreTake(xResourceMutex, portMAX_DELAY) == pdTRUE) {
            UART_SendString("    [HIGH] EXECUTION AVEC HERITAGE PRIO !\r\n");
            xSemaphoreGive(xResourceMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(800));
    }
}

int main(void) {
    UART_Init(9600);
    _delay_ms(100);
    UART_SendString("=== FreeRTOS Priority Inversion Demo ===\r\n\r\n");

    xResourceMutex = xSemaphoreCreateMutex();

    if (xResourceMutex != NULL) {
        /* Alignement sécurisé de la pile */
        xTaskCreate(vTaskLow,    "Low",   70, NULL, 1, NULL);
        xTaskCreate(vTaskMedium, "Med",   70, NULL, 2, NULL);
        xTaskCreate(vTaskHigh,   "High",  70, NULL, 3, NULL);

        vTaskStartScheduler();
    }

    while (1);
    return 0;
}