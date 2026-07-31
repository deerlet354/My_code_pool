create_clock -period 10.000 -name sysclk [get_ports sysclk_i]
set_property PACKAGE_PIN L5 [get_ports sysclk_i]
set_property IOSTANDARD LVCMOS33 [get_ports sysclk_i]
set_property BITSTREAM.GENERAL.COMPRESS true [current_design]

#RX TO USB 232 IC TX
set_property PACKAGE_PIN R4 [get_ports uart_tx_o]
set_property IOSTANDARD LVCMOS33 [get_ports uart_tx_o]

#bit compress 
set_property CFGBVS VCCO [current_design] 
set_property CONFIG_VOLTAGE 3.3 [current_design] 
set_property BITSTREAM.GENERAL.COMPRESS true [current_design]

