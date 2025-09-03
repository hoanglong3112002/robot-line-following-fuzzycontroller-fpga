# HDL Code – Top-Level Design

Target Device: **Cyclone IV E – EP4CE6F17C8**

This section presents the top-level HDL module for the autonomous line-following robot.  
The design integrates FPGA pins with the Qsys (Platform Designer) system generated in Quartus, which includes Nios II processor, UART, sensor, and motor control peripherals.

---

## Top-Level Module

```verilog
module test_do_an(
    input CLOCK_50, 
    input [1:0] KEY,
    input [4:0] SENSOR,
    input [0:0] ENCODER1, ENCODER2,
    output [0:0] IN1, IN2, IN3, IN4,
    output UART_TX,
    input UART_RX
); 
    nios_sys u0 (
        .clk_clk                             (CLOCK_50),                           
        .reset_reset_n                       (KEY[0]),                  
        .sensor_external_connection_export   (SENSOR),  
        .encoder1_external_connection_export (ENCODER1), 
        .encoder2_external_connection_export (ENCODER2), 
        .in1_external_connection_export      (IN1),     
        .in2_external_connection_export      (IN2),      
        .in3_external_connection_export      (IN3),    
        .in4_external_connection_export      (IN4),     
        .key1_external_connection_export     (KEY[1]),               
        .uart_external_connection_rxd        (UART_RX),        
        .uart_external_connection_txd        (UART_TX)	  
    );
endmodule

