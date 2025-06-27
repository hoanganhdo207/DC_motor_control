# ⚙️ DC Motor Control with PID – Speed and Position
This project demonstrates how to control a DC motor's speed and position using the PID control algorithm. By continuously adjusting the motor's input based on feedback from a encoder sensor, the system can maintain precise speed and move to target positions with high accuracy.

## 🔧 Key Features
  - Real-time speed control using PID
  - Position control through incremental encoder feedback
  - Configurable PID parameters for tuning and system response optimization
# 🧠 System design
We use ATmega32 microcontroller for this project - an affordable 8-bit MCU that offers decent processing speed , making it well-suited for simple embedded systems. While it doesn't have the high performance or memory of more advanced boards like ESP32, STM32, ... , it is perfectly adequate for projects that don't require intensive computation or large storage. Its reliability and ease of use make it a great fit for real-time control tasks like PID motor control.  

![thumbnail](https://raw.githubusercontent.com/hoanganhdo207/DC_motor_control/main/images/vi-dieu-khien-atmega32-1-599x420.jpg) 

## 🔥 Burn bootloader 
If you buy a brand-new ATmega32, it's often necessary to burn the bootloader. The bootloader is a small piece of code that allows the microcontroller to receive and run programs from the computer via a serial interface. Without it, direct programming via ISP (In-System Programming) is required for every update. Burning the bootloader sets up the microcontroller’s fuse bits and prepares it for easier future uploads.

## 📟 LCD Display Integration
To provide real-time feedback during operation, we integrated a 16x2 LCD display using the I2C interface. The display shows important parameters such as current speed, position, and PID values. This helps in monitoring the system’s behavior and adjusting the control loop more effectively during testing and tuning.

![thumbnail](https://raw.githubusercontent.com/hoanganhdo207/DC_motor_control/main/images/lcd.jpg) 

## 📸 Previews
[Video demo](https://drive.google.com/file/d/16V3UMsNc82UP_47mTUNTjgNMJ04aVsQA/view?usp=sharing)

## 📄 License
This project is licensed under the [MIT License](LICENSE).  
You are free to use, modify, and distribute this software in both personal and commercial projects, as long as the original license is included.

## 📬 Contact
For questions or collaboration:
  - Name: Đỗ Hoàng Anh
  - Email: hoanganhdo207@gmail.com
  - GitHub: [hoanganhdo207](https://github.com/hoanganhdo207)




