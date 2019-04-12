#ifndef __CUSTOM_GPIO__H
#define __CUSTOM_GPIO__H


#define GREEN_LED_PIN  GPIO_PIN_13
#define RED_LED_PIN  GPIO_PIN_14
#define USER_BUTTON GPIO_PIN_0


#define TOGGLE_RED_LED()   GPIOG->ODR ^= RED_LED_PIN
#define TOGGLE_GREEN_LED() GPIOG->ODR ^= GREEN_LED_PIN
#define BUTTON_READ()      GPIOA->IDR & USER_BUTTON

void cgpio_init(void);



#endif /*__CUSTOM_GPIO__H */
