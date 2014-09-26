set ns [new Simulator] 
source tb_compat.tcl 
# Nodes 
set node0 [$ns node] 
tb-set-node-os $node0 Ubuntu1004-STD 
tb-add-node-attribute $node0 containers:node_type qemu
tb-add-node-attribute $node0 "containers:PartitionPass" 0
set node3 [$ns node] 
tb-set-node-os $node3 Ubuntu1004-STD 
tb-add-node-attribute $node3 containers:node_type qemu
tb-add-node-attribute $node3 "containers:PartitionPass" 1
set node4 [$ns node] 
tb-set-node-os $node4 Ubuntu1004-STD 
tb-add-node-attribute $node4 containers:node_type qemu
tb-add-node-attribute $node4 "containers:PartitionPass" 1
set node5 [$ns node] 
tb-set-node-os $node5 Ubuntu1004-STD 
tb-add-node-attribute $node5 containers:node_type qemu
tb-add-node-attribute $node5 "containers:PartitionPass" 0
set node6 [$ns node] 
tb-set-node-os $node6 Ubuntu1004-STD 
tb-add-node-attribute $node6 containers:node_type qemu
tb-add-node-attribute $node6 "containers:PartitionPass" 0
set rtr1 [$ns node] 
tb-set-node-os $rtr1 Ubuntu1004-STD 
tb-add-node-attribute $rtr1 containers:node_type qemu
tb-add-node-attribute $rtr1 "containers:PartitionPass" 1
set rtr2 [$ns node] 
tb-set-node-os $rtr2 Ubuntu1004-STD 
tb-add-node-attribute $rtr2 containers:node_type qemu
tb-add-node-attribute $rtr2 "containers:PartitionPass" 1
set usRTR [$ns node] 
tb-set-node-os $usRTR Ubuntu1004-STD 
tb-add-node-attribute $usRTR containers:node_type qemu
tb-add-node-attribute $usRTR "containers:PartitionPass" 1

# Links 
set link0 [$ns duplex-link $node0 $usRTR 1000000.0kb 0.0ms DropTail] 
tb-set-ip-link $node0 $link0 10.10.0.1 
tb-set-ip-link $usRTR $link0 10.10.0.2 
set link1 [$ns duplex-link $node5 $rtr2 1000000.0kb 0.0ms DropTail] 
tb-set-ip-link $node5 $link1 10.1.2.3 
tb-set-ip-link $rtr2 $link1 10.1.2.2 
set link2 [$ns duplex-link $node6 $rtr2 1000000.0kb 0.0ms DropTail] 
tb-set-ip-link $rtr2 $link2 10.1.3.2 
tb-set-ip-link $node6 $link2 10.1.3.6 
# Lans 
set lan0 [$ns make-lan "$rtr1 $rtr2 $usRTR" 1000000.0kb 0.0ms] 
tb-set-ip-lan $rtr1 $lan0 10.99.0.1 
tb-set-ip-lan $rtr2 $lan0 10.99.0.2 
tb-set-ip-lan $usRTR $lan0 10.99.0.3 

set lan1 [$ns make-lan "$rtr1 $node3 $node4" 1000000.0kb 0.0ms] 
tb-set-ip-lan $rtr1 $lan1 10.1.0.1 
tb-set-ip-lan $node3 $lan1 10.1.0.3 
tb-set-ip-lan $node4 $lan1 10.1.0.4 
$ns rtproto Manual 
$ns run 