# Systems Design Notes

## Harvard CS75 Video on Scalability

[link](https://www.youtube.com/watch?v=-W9F__D3oY4)

* **Vertical Scaling**: buy better servers with more cores, install more ram and disk space on your computer.
    * Problem - will eventually reach limit (in terms of money or state of the art).

* **Horizontal Scaling**: Using many instances of the same hardware.
    * Need to build system to be able to distribute tasks.

* Vertical scaling refers to getting **one** "really good" machine, while horizontal scaling refers to building an architecture to support **many** machines. Due to limitations, most systems solutions are for allowing horizontal scaling to work better.

* **Load Balancer** - a "black box" that receives requests from clients and routes the requests to different servers. From the outside, clients can only contact the load balancer (it uses a public address) while the underlying servers can't be contacted from the outside (they use a private IP address).
    * Load balancer can route requests based on several factors, such as the current load of each server, splitting up the type of work each server does, or just loops through the servers are requests are received (this is called round robin).
    * With multiple servers, any state that needs to be stored outside the current session (such as website cookies) need to be stored in a central location

* **Sticky Sessions** - Some state (like cookies) need to be accessed across multiple servers since the user can be routed to different servers for different requests
    * Storing this shared state in a central server like the load balancer.
        * **RAID** - replication within a single server. This makes storing all information on a single server (like the load balacncer) more stable.
            * Raid0 - Data is saved by "striping" it across two drives, which makes it faster to write data.
            * Raid1 - Data is written and copied over two harddrive, so if one dies the data is still there.
            * Raid10 - Combines Raid0 and Raid10 using 4 hard drives
            * Raid5 and Raid6 are middle grounds between Raid0/1 and Raid10.

    * MySQL databases for storing data in a centralized place
        * But is not able to store information on a specific session

    * The cookies itself can contain information like the server it was originally from. Specifically, it could contain a random number which the load balancer can map to a specific server.

* **Caching**
    * Can cache generated html files (i.e. static webpages) when reads more common than writes.
        * Downsides - need to store content like headers which is redundant.
        * Also hard to change the aesthetics of the files without regenerating all files.
    * MySQL query cache - database queries are cached so that second time a query is run is faster.
    * Memcache - Software ran on server that caches data in RAM.
        * LRU can be used if cache runs out of memory




* Database Optimizations and Replication
    * **Replication** - making multiple copies of data stored in a database so that if one database goes down the data is still available.
        * **Master-Slave Replication** - there is a master database/server which serve requests, and the data is copied to multiple slave databases that can be promoted to master if the master goes down.
            * Reads can be routed to the slave databases since nothing has to propogate to other servers.
            * Load balancers can be used to balance read requests across the slave servers.

        * **Master-Master Replication** - There are two master servers that can both process requests and each one has a copy of the data that is propogated to the other.
            * This can be done by "heartbeat" checks between the two servers, and is called **High Availability**.


    * **Partitioning** - have different servers for different tasks. For example, in Facebook (early stage), have one server for Harvard users and one server for MIT users.


## Scalability for Dummies article
[link](https://www.lecloud.net/tagged/scalability/chrono)

**Part 1 Clones**
* We can first scale horizontally by increasing the number of web servers. To do this, we need to make sure that persistent state is stored in a central database. All the servers should be running the same code.

**Part 2 Database**
* The database now becomes the bottleneck since there is only one database that all the servers access.
* One solution is to use master-slave replication and add more ram to the database. However, this gets expensive as the amount of data increases.
* Another solution is to use a NoSQL database. 

**Part 3 Cache**
* You can now store lots of data but the database is slow. In-memory caches like Redis and Memcached are key-value stores that act as buffers between database and application and is much faster to access than the db. There are two types of caching patterns:
    * Cached database queries - store the result of a query in the cache.
    * Cache objects - store classes and arrays in the cache. Examples include:
        * User sessions
        * HTML code.
    * Second method is better because database queries can become complex.

**Part 4 Asynchronism**
* One method is to do time consuming work in advance and serve the finished work quickly.
    * Example - turn dynamic content into static content.
* Another method is to use a queue for requests that is constantly checked by workers.

## Tradeoffs

### Performance vs Scalability (not really a tradeoff, just separate definitions)
* Performance refers to something that runs fast for one user.
* Scalability refers to something that is able to run fast for multiple users
or for large inputs.
    * More specifically, a system is scalable if performance incrases proportionally with resources.

### Latency vs Throughput
* Latency - time required to perform an action
* Throughput - number of actions executed per unit of time.

### Availability and Consistency
* We can either have Consistency or Availability in the presence of a Partition (CAP theorem).
    * Example - Two bank ATM's with partition between them
        * Availability - ATM's will always accept transactions but will eventually update the other ATM when the partition heals.
        * Consistency - ATM will block until it's able to contact all other ATM's.

* CP - The server will wait for a response from other servers. The request my return an error or time out if nothing is received.
* AP - The system will serve reads with the most recent data it has, and server writes by storing it and sending it out later when it can.

## Levels of Consistency (weakest to strongest)
* Weak consistency - after a write, a read is not guranteed to see it. For example, a cache (without a db attached to it). Example: first person shooter game
* Eventual consistency - after a write, reads will eventually see it (example: email), but not immediately after an ack.
* Strong consistency - Reads will always see a write after the write returns (i.e. the server acks)

## Availability Patterns
* Master-slave or master-master servers - one server (or two) serve all the requests, and there is backups that can become active if the master fails.


## Domain Name System (DNS)
* Service that translates domain names (like google.com) to ip addresses of servers that can be sent requests.
    * DNS is hierarchical, with authoritative servers at the top level and at the lower levels, servers that cache results from the top-level servers.
    * There are services for routing traffic, such as round robin, latency-based, and geography based. I.e. load balancing Algorithms


* 5 common load-balancing algorithms:
    * **Round Robin** - requests are routed to servers one after another, looping back to the first router. 
    * **Weighted Round Robin** - servers with more resources are weighted more heavily in the Round Robin scheme.
    * **Least Connections** - servers with the least number of connections (with clients) are given priority for new requests
    * **Weighted Least Connections** - least connections but servers are weighted based on capacity.
    * **Random** - servers are assigned randomly

## Content Delivery Network (CDN)
* A CDN is a network of servers around the world that is used to serve static data to nearby users.
* Pull CDN - during a first request, the CDN pulls the static data from the original server and caches it. Cached data is valid until some timer runs out (time-to-live).
    * Good for low traffic sites, but users may not see most updated stuff
* Push CDN - static content is uploaded to the CDN to pull when its needed. All data needed must be uploaded since the CDN can't pull.
    * Good for unchanging content

## Load Balancer
* Load balancers distribute client requests to servers or databases. They can also serve the following functions:
    * Encrypt responses and decrypt requests
    * Issue cookies and route client requests to the same server if there is no central state for sticky sessions.

* For safety, multiple load balancers can be used such as in active-passive or active-active mode.

* More load balancing algorithms:
    * **Layer 4 load balancing** - load balancer looks at transport layer (things like destination IP address and ports, but not contents of the request) to servers.
    * **Layer 7 load balancing** - load balancer looks at the application layer (header, message, cookies).

## Reverse Proxy
* A reverse proxy accepts requests from a client and forwards it to a server that can fulfill it, returning the server's response to the client.
    * Increases security by hiding information about backend servers.
    * Increases scalability and flexibility of backend server infrastructure.
    * Can also be used for compression, encryption, and caching requests
* Some products like NGINX and HAProxy can do both reverse proxying and load balancing

## Application Layer (or platform layer)
* A platform layer is a layer of servers that go between the web server and the database, which is responsible for serving database requests.
    * Allows the logic for database requests to be separated from business logic.
    * Can scale databases without affecting web servers.
    * Can modify business logic and API in web servers without modifying the database query servers.

* There are many definitions for application layer. Some definitions mean that the application layer translates UI to business logic and back. Other definitions include that application servers are responsible for serving dynamic content, while web servers serve mainly static content.

* Another architecture is microservices, where each service runs a unique process separate/independent from other services.
    * Service discovery is needed to keep track of services. This can be done with things like Zookeeper.
    * Health checks (send a simple request and get a reply) are used to check that services are up.

## Database

### Relational Database Management Systems (RDBMS)
* Data is organized in tables (example: SQL)
* Transactions satisfy ACID properties:
    * Atomicity - transactions are atomic (either the full transaction occurs or nothing occurs)
    * Consistency - database invariants are met before and after each transaction.
    * Isolation - concurrent transactions do not conflict with one another.
    * Durability - transactions are stored in non-volatile memory so that if the system fails the data is still there.

#### Scaling Techniques:
* Replication - the stored data is replicated across many database servers.
    * Master-slave - a master server serves all writes and propogates updates to slaves. Slave servers can serve reads. Slaves are promoted to mastesr if master dies.
        * Problem - data loss if master fails before new data is fully replicated to the slaves.
    * Master-master - two master servers sync with each other and both can serve reads and writes.
        * Problems - can invalidate consistency between masters. Or can have consistency at the cost of availability.

* Federation - splits up one big database into smaller databases by function/context.
    * Benefits - less traffic per database and more cache hits - overall better throughput.
    * Disadvantages - application needs to determine which database to read/write from and joins from multiple databases may be needed.

* Sharding - distributes data across multiple databases that is each responsible for a portion of the data (such as splitting usernames by first letter).
    * Advantages and disadvantages similar to federation.
    * Use **consistent hashing** to make shards balanced even if a shard dies:

* Denormalization - store redundant copies of database data so that joins can reads can be calculated faster. However, need to introduce constraints so that database is consistent, which makes writes slower.

### NoSQL
* SQL (relational) databases do not work well over several distributed servers since a request often needs to access several tables on the database, and each table needs to be stored in one machine.
* NoSQL (not only SQL) are databases that use a different format than relational databases. They often favor availability over consistency, and satisfy BASE:
    * Basically available - system is available most of the time
    * Soft state - state of the system may change over time, even without input (because of eventual consistency)
    * Eventual consistency - same definition as above.

* Data is usually stored on a large number of commodity machines and are partitioned/replicated.
* Aggregation model - data stored in a document or value in a key-value store can have relevant information as an aggregate, unlike in SQL where the data may need to be obtained from various locations.
* Schemaless model - data schema (organization) can be changed easily, unlike in relational databases.
* Usually can handle a lot more requests, and for a larger amount of data. It is designed for scalability, not consistency.

    
#### Key-value stores
* Allows for O(1) reads and writes, and is useful for rapidly changing data. But more complex operations (like joins) will need to be done in application layer.
* Only supports get and put operations
* Example: Redis
    * Redis is an in-memory key value store. Thus it can only store small data that needs to be accessed quickly.
    * Persistence - since memory is volatile, Redis must save the data somewhere so that it still exists when the server is shut down:
        * RDB - copy all data to an RDB in every interval.
        * AOF (append-only file) logging - expensive because requires lots of storage and disk accesses.
        * SAVE command - creates an RDB snapshot.
    * Replication - uses master-slave replication. Slaves can be configured to write data to persistent memory to save time for master.
    * Sharding - data can be sharded across many computers, each of which can have a master-slave replication scheme with other computers.
* Example: Memcache

    


#### Document stores
* A key value store that restricts values to structured formats like JSON documents.
    * Allows for fetching parts of a document.
* Example: MongoDB
* Example: CouchDB
    * 

#### Wide-Column stores
* Multi-level (often 2 level) map.
* Allows for sparse data to be stored in less space.