# TCL File Generated by Component Editor 12.1sp1
# Sat Mar 12 15:55:41 MST 2016
# DO NOT MODIFY


# 
# LED_Detector "LED_Detector" v1.0
# null 2016.03.12.15:55:41
# 
# 

# 
# request TCL package from ACDS 12.1
# 
package require -exact qsys 12.1


# 
# module LED_Detector
# 
set_module_property NAME LED_Detector
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property DISPLAY_NAME LED_Detector
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property ANALYZE_HDL AUTO
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL LED_detector
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
add_fileset_file LED_detector.vhd VHDL PATH vhdl/LED_detector.vhd


# 
# parameters
# 
add_parameter IDW INTEGER 23
set_parameter_property IDW DEFAULT_VALUE 23
set_parameter_property IDW DISPLAY_NAME IDW
set_parameter_property IDW TYPE INTEGER
set_parameter_property IDW UNITS None
set_parameter_property IDW ALLOWED_RANGES -2147483648:2147483647
set_parameter_property IDW HDL_PARAMETER true
add_parameter ODW INTEGER 23
set_parameter_property ODW DEFAULT_VALUE 23
set_parameter_property ODW DISPLAY_NAME ODW
set_parameter_property ODW TYPE INTEGER
set_parameter_property ODW UNITS None
set_parameter_property ODW ALLOWED_RANGES -2147483648:2147483647
set_parameter_property ODW HDL_PARAMETER true


# 
# display items
# 


# 
# connection point clock
# 
add_interface clock clock end
set_interface_property clock clockRate 0
set_interface_property clock ENABLED true

add_interface_port clock clk clk Input 1


# 
# connection point reset
# 
add_interface reset reset end
set_interface_property reset associatedClock clock
set_interface_property reset synchronousEdges DEASSERT
set_interface_property reset ENABLED true

add_interface_port reset reset reset Input 1


# 
# connection point avalon_streaming_sink
# 
add_interface avalon_streaming_sink avalon_streaming end
set_interface_property avalon_streaming_sink associatedClock clock
set_interface_property avalon_streaming_sink associatedReset reset
set_interface_property avalon_streaming_sink dataBitsPerSymbol 8
set_interface_property avalon_streaming_sink errorDescriptor ""
set_interface_property avalon_streaming_sink firstSymbolInHighOrderBits true
set_interface_property avalon_streaming_sink maxChannel 0
set_interface_property avalon_streaming_sink readyLatency 0
set_interface_property avalon_streaming_sink ENABLED true

add_interface_port avalon_streaming_sink stream_in_data data Input "(idw) - (0) + 1"
add_interface_port avalon_streaming_sink stream_in_startofpacket startofpacket Input 1
add_interface_port avalon_streaming_sink stream_in_endofpacket endofpacket Input 1
add_interface_port avalon_streaming_sink stream_in_valid valid Input 1
add_interface_port avalon_streaming_sink stream_in_ready ready Output 1


# 
# connection point avalon_streaming_source
# 
add_interface avalon_streaming_source avalon_streaming start
set_interface_property avalon_streaming_source associatedClock clock
set_interface_property avalon_streaming_source associatedReset reset
set_interface_property avalon_streaming_source dataBitsPerSymbol 8
set_interface_property avalon_streaming_source errorDescriptor ""
set_interface_property avalon_streaming_source firstSymbolInHighOrderBits true
set_interface_property avalon_streaming_source maxChannel 0
set_interface_property avalon_streaming_source readyLatency 0
set_interface_property avalon_streaming_source ENABLED true

add_interface_port avalon_streaming_source stream_out_data data Output "(odw) - (0) + 1"
add_interface_port avalon_streaming_source stream_out_startofpacket startofpacket Output 1
add_interface_port avalon_streaming_source stream_out_endofpacket endofpacket Output 1
add_interface_port avalon_streaming_source stream_out_valid valid Output 1
add_interface_port avalon_streaming_source stream_out_ready ready Input 1


# 
# connection point thres_active_conduit
# 
add_interface thres_active_conduit conduit end
set_interface_property thres_active_conduit associatedClock clock
set_interface_property thres_active_conduit associatedReset reset
set_interface_property thres_active_conduit ENABLED true

add_interface_port thres_active_conduit threshold_active export Input 1


# 
# connection point thres_range_conduit
# 
add_interface thres_range_conduit conduit end
set_interface_property thres_range_conduit associatedClock clock
set_interface_property thres_range_conduit associatedReset reset
set_interface_property thres_range_conduit ENABLED true

add_interface_port thres_range_conduit threshold_range export Input 8


# 
# connection point interrupt_sender
# 
add_interface interrupt_sender interrupt end
set_interface_property interrupt_sender associatedAddressablePoint avalon_slave_0
set_interface_property interrupt_sender associatedClock clock
set_interface_property interrupt_sender associatedReset reset
set_interface_property interrupt_sender ENABLED true

add_interface_port interrupt_sender irq irq Output 1


# 
# connection point avalon_slave_0
# 
add_interface avalon_slave_0 avalon end
set_interface_property avalon_slave_0 addressUnits WORDS
set_interface_property avalon_slave_0 associatedClock clock
set_interface_property avalon_slave_0 associatedReset reset
set_interface_property avalon_slave_0 bitsPerSymbol 8
set_interface_property avalon_slave_0 burstOnBurstBoundariesOnly false
set_interface_property avalon_slave_0 burstcountUnits WORDS
set_interface_property avalon_slave_0 explicitAddressSpan 0
set_interface_property avalon_slave_0 holdTime 0
set_interface_property avalon_slave_0 linewrapBursts false
set_interface_property avalon_slave_0 maximumPendingReadTransactions 0
set_interface_property avalon_slave_0 readLatency 0
set_interface_property avalon_slave_0 readWaitTime 1
set_interface_property avalon_slave_0 setupTime 0
set_interface_property avalon_slave_0 timingUnits Cycles
set_interface_property avalon_slave_0 writeWaitTime 0
set_interface_property avalon_slave_0 ENABLED true

add_interface_port avalon_slave_0 avs_read read Input 1
add_interface_port avalon_slave_0 avs_address address Input 3
add_interface_port avalon_slave_0 avs_chipselect chipselect Input 1
add_interface_port avalon_slave_0 avs_readdata_centroid readdata Output 32
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isFlash 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment avalon_slave_0 embeddedsw.configuration.isPrintableDevice 0

