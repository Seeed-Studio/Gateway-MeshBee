<Gateway-MeshBee>
---------------------------------------------------------

### Usage:
A MeshBee(seeed's zigbee) gataway project based on Seeeduino Arch Pro

A common scenario in a wireless network is the need for most network nodes to communicate with
a single node which performs some centralised function.
In this demo project,we implement a Client-Server model. AT commands "ATQD" send from client to server
(In this case,coordinator binded on gateway is treated as client,and end device is treated as server)
and then,sensor attributes returned from server to client.<br>

Gateway+Coordinator <-----------------> Router or End Device

A on-chip temperature measurement was performed on server, data was transmit to xively,a world famous sensor 
cloud platform.

Hardware:<br>
        1.Seeeduino Arch Pro(mBed);<br>
        2.Mesh Bee;<br>
        3.Xbee Shield<br>
        
Software:<br>
        1.RTOS;<br>
        2.Zigbee Pro;<br>
        3.Xively lib<br>

In order to improve Mesh-Bee firmware Project,please feel free to contact me with your suggestions.

For more information, please refer to [wiki page](<http://www.seeedstudio.com/wiki/Mesh_Bee>).

    
----

This software is written by <long.wang@seeedstudio.com> for seeed studio<br>
and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>
