# server_client_communication
Write  a  simulation  for  a  burger  joint  and  its customers.  The  burger  joint  should  be implemented  as  a  server  application,  while  the clients  should  be  implemented  as clientapplicationsconnecting  to  the  server  via  TCP/IP. The  burger  joint  will  have several  chefs producing  the  burgers.Each  burger  will  take  some  time  to  be  produced  and  the  clients  (see customers) willeat the burgers if they are available.However,they also will need some time to consume the burgers. The burger joint can produce a certain number of burgers and each client is capable to consume a certain amount of burgers.The simulation stops either when there are no more burgers available or each of the clients consumed the maximum number of burgers they were supposed to eat. If there are still leftsome burgers (or some of the burgers werenot yet preparedby the chefs), the burger joint remains open waiting for possible new customers. 