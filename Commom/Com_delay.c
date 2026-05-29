#include "Com_delay.h"

void HAL_Delay(uint32_t Delay)
{
    if (Delay == 0U)
    {
        return;
    }

    if ((xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) && (__get_IPSR() == 0U))
    {
        if (Delay == HAL_MAX_DELAY)
        {
            vTaskDelay(portMAX_DELAY);
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(Delay));
        }
        return;
    }

    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;

    if (wait < HAL_MAX_DELAY)
    {
        wait += (uint32_t)HAL_GetTickFreq();
    }

    while ((HAL_GetTick() - tickstart) < wait)
    {
    }
}
