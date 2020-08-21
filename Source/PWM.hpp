



/*!< Specifies the prescaler value used to divide the TIM clock.
This parameter can be a number between 0x0000 and 0xFFFF */
#define PWM_PRSC			24000

/*!< Specifies the period value to be loaded into the active
Auto-Reload Register at the next update event.
This parameter must be a number between 0x0000 and 0xFFFF.*/ 
#define PWM_PERIOD			240

/*!< Specifies the repetition counter value. Each time the RCR downcounter
reaches zero, an update event is generated and counting restarts from the RCR value (N).
This means in PWM mode that (N+1) corresponds to:
   - the number of PWM periods in edge-aligned mode
   - the number of half PWM period in center-aligned mode
This parameter must be a number between 0x00 and 0xFF. 
@note This parameter is valid only for TIM1 and TIM8. */
/* (N.b): Internal current loop is performed every 
             (REP_RATE + 1)/(2*PWM_FREQ) seconds.
 REP_RATE has to be an odd number in case of three-shunt
 current reading; this limitation doesn't apply to ICS*/
#define REP_RATE			1

/*!< Specifies the TIM Output Compare pin state during Idle state.
This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
@note This parameter is valid only for TIM1 and TIM8. */
#define LOW_SIDE_POLARITY	TIM_OCIdleState_Reset

/*!< Specifies the delay time between the switching-off and the
switching-on of the outputs.
This parameter can be a number between 0x00 and 0xFF  */
#define DEADTIME			0



class CPWM
	{
	public:
	CPWM();
	~CPWM();
	
	void SetMotorSpeed(signed char speed);	//range of values from -100 up to +100 (percent)
	
	void SetOutEnable(bool state);
	bool GetOutEnable(void);
	//private:
	};