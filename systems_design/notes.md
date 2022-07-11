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
    * Each data item is a row in the table, each of which contains multiple columns.
        * Rows in the same table have the same columns
    * Each column must contain the same type of value (defined by the schema)
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
        * Can be configured to have high availability or high consitency (but not both obviously)
    * Sharding - data can be sharded across many computers, each of which can have a master-slave replication scheme with other computers.
        * Redis uses hash slots or virtual nodes, together with consistent hashing - partition hash space into virtual nodes, and each node is responsible for some number of virtual nodes.
* Example: Memcache
    * Memcache is implemented like a hash map, and all operations are O(1).
    * Memcache memory is allocated at the start (the user specifies how much to use), uses LRU to evict data.
    * Memcache uses its own memory (instead of malloc and free) to avoid memory fragmentation after many rights/moving. This memory allocation is called slabs.
        * The memory is divided into pages of 1mb each (also the size of the largest thing that can be stored). There are slab classes of various sizes.
        * Each slab class represents how a page will be partitioned (into chunks of equal size).
        * Each slab class gets one page at the start.
        * New objects are written to the smallest chunk that can take it. When there are no available chunks for a slab class, a new page can be assigned.
        * If all pages are used up, LRU is used. There is an LRU for each slab class.
    * Memcache will automatically use multiple servers through consistent hashing (sharding).

    


#### Document stores
* A key value store that restricts values to structured formats like JSON documents.
    * Allows for fetching parts of a document.
    * Documents are often grouped together by tags, metadata, directories, etc.
* Example: MongoDB
* Example: CouchDB
    * Uses sharding and consistent hashing to cluster servers. There are multiple shards and multiple servers per shard, both of which are specified by the user.
    * When updating database, the server will wait until half the servers for that shard return.
    * Not guranteed to be consistent, and may return results before what occurred.
* Example: Elasticsearch
    * Elasticsearch is a document store that allows for fast searching performance when searching with strings within documents.
    * Elasticsearch uses inverted indexes - maps words within documents to the sets of documents that these words are in. The keys (words) are sorted for quick searching.
        * Also stores indexes for reverses of words and other processed words for quick searching of suffixes, prefixes, etc. Essentially inverted index allows for searching of prefixes, so we have to find tricks to make everything a prefix problem.
    * Index changes from documents are first stored in memory. New document additions are buffered in memory and eventually the entire index is flushed to disk.
        * Updates are processed as a deletion and then an addition.
        * Each file is immutable. Deletes are added to a separate file and processed while searching.
        * Each flush of documents create a new index segments.
    * Lucene indexes (a part of a whole elasticsearch index) are spread across multiple segments called index segments, which are spread across multiple files on disk.
        * Searches are performed across all segments and then results are merged.
        * When there are too many segments, the segments themselves can be merged (and deleted documents will be actually removed).
    * Elasticsearch indexes are spread across shards, which individually are lucene indexes and can have multiple replicas.
        * Searches are conducted over all shards.
        * Documents are routed to shards via round robin by defaulted, but this can be customized.
    * Transactions (document additions) are logged in persistent storage in case of a crash.


#### Wide-Column stores
* Multi-level (often 2 level) map.
* Allows for sparse data to be stored in less space. Offers high availability and consistency, useful for very large datasets.
* Example: Google's BigTable
    * Data is stored in tables, which can be accessed by a row key, column key, and for each cell there are multiple values corresponding to different timestamps.
    * Data for each column is stored contiguously, in order of row key, so locality of row keys can be utilized.
    * Columns are grouped into column families. Usually there are only a few column families, but each family could have a lot of columns.
    * Data for each table is split across row key ranges into tablets, which are sharded across servers.
    * Each bigtable instance has one master server, which assigns tablets to tablet servers and does garbage collection and load balancing.
    * Each tablet server is responsible for serving reads/writes to that tablet (clients usually don't communicate with master, but instead talk directly to tablet servers). 
* Example: Apache's HBase

* Example: Facebook's Cassandra

#### Graph Databases
* Each node represents a data record and each edge is a relationship between nodes.


#### SQL vs NoSQL
* Advantages of SQL:
    * Joins are easier to do, but slower if data is replicated or denormalized in NoSQL
    * Data is organized in a schema and enforces the schema
    * Transactions are ACID
* Disadvantages of SQL:
    * Hard to scale to multiple servers while still maintaining ACID.
    * Data has to fit in a predefined structure - can lead to sparse data.
    * Relational database doesn't work with large files like images or videos or documents.
* Example data suited for NoSQL:
    * log data or clickstream data.
    * Temporary data like shopping carts.

## Caches
* Caches store data (temporarily) in a more accessible place than databases, so that the data can be served quickly.
* Examples:
    * CDN's are caches that often serve static web page data.
    * On a web server, requests can be cached by both the client and server.
    * Databases usually have a cache
    * Redis and Memcached are key-value stores that can be used as cache in the application level. (The data is held in RAM so it is fast to access)

* Cache read update types:
    * Cache-aside (look-aside cache)
        * The cache is not connected to storage and cacheing is handled by the client/application.
        * The application first looks at the cache to see if the data is there. If not, it looks in the storage and gets the data, then updates the cache.
        * Cache staleness determined by TTL, or by write through
        * Problems: Long latency because each cache miss results in three RPC's. But the system is resistant to cache failures.
    * Read-through
        * Cache is connected to a database. When there is a miss, the cache will read from the database and store in cache.
    * Refresh-ahead
        * The cache will automatically refresh data from the DB before TTL expires.
        * Performance depends on whether the same items are needed multiple times in the cache (no point in refreshing stuff that isn't needed).

* Cache write update types:
    * Write-through
        * The application will always read and write directly to cache.
        * On a write, the cache will synchronously propogate the data to the database, and return after the db write is done.
        * Problems: Writes are slow (but reads are fast)
    * Write-behind (write-back)
        * Same as write through except writes are done asynchronously to the database.
        * This could be done through a task queue.
        * Problems - there could be incoonsistent data if the cache returns and then dies.
    * Write-around
        * Data is written directly to the database, and not the cache.

## Asynchronism
* Refers to reducing request times by doing expensive operations either before or after requests.

* Message Queues
    * Used to receive, hold, and deliver messages which can then signal a work to do some job.
    * For example, when a tweet is posted, the tweet can be put on a message queue, which can be handled by some worker that puts the tweet in a database.
    * Queues require a message broker to assign queue items to workers. This is to prevent race conditions when workers read directly from the queue. This is done specifically by "Task Queues", like Celery.
    * Examples: Redis Queue, RabbitMQ, Amazon SQS, celery


* Back Pressure - 
    * Little's Law - states that the average number of items in the queue (or in any process) is equal to the arrival rate (items/second) times the average length of processing for each item (seconds).
    * We can make sure queues maintain a maximum latency by bounding the size of queues and reject new tasks when the threshold is exceeded.
        * This is called back pressure.
        * Threshold is determined using Little's law, based on the time to process each task, the number of threads, and the max latency desired.
    * Without backpressure, the queue can get too large and suck away resources like RAM from other tasks, causing the system to crash (out of memory error).

## Communication

* Interconnection layers (OSI 7 layer  mode) from high to low:
    * Application layer - Actual client-server interactions for sending and receiving data.
    * Presentation layer - encryption and compression of data, translates between user readable data and network readable data.
    * Session layer - determines the ports used for sending data so that the right application gets it.
    * Transport layer - TCP or UDP, the protocol for sending the message.
    * Network layer - Determines the route the data is sent rhough
    * Data link layer and Physical layer - transfers data bits through the cable/wireless.

### HTTP (Hypertext transfer protocol)
* Used at the application layer and relies on underlying network protocols like TCP or UDP.
* Method for transporting data between client and server. Clients issue requests and servers issue responses.
* Each request consists of a verb and endpoint. Verbs consists of GET (read), POST (create), PUT (update), PATCH (update), DELETE, and more.
* Response includes the desired data as well as status information.

### TCP and UDP
* TCP - transmission control protocol
    * TCP is connection based (a connection is formed between two machines).
    * TCP is reliable - all data is guranteed to arrive at the other end and in the same order it was written.
        * TCP will wait for an ack from the receiving end, and will resend packets if none is received.
        * Duplicate packets on receiving end are discarded, and out of order packets are resequenced at the end.
        * Thus TCP is slower than UDP
    * TCP sends through streams and will automatically break up data into packets
    * 

* UDP - user datagram protocol
    * UDP does not use a connection between to machines - the receiver will listen on a port and receive packets from any computer.
    * UDP is unreliable and packets may not be sent in the right order.
    * UDP is faster than TCP and has lower latency - good for things like fps games
    * UDP also supports broadcast transmission - it sends to all devices on the subnet
    * Does not send through streams, will directly send packets individually (datagrams)

* TCP and UDP shouldn't be mixed because they will affect each other on the network
    * TCP increases packet loss in UDP


### RPC and REST
* RPC and REST are different architectures for API's. Both use the HTTP protocol.
* RPC (remote procedure call)
    * A client calls a procedure to be executed on a different server or address space, and the result is returned to the caller.
    * Usually used for internal communications, as clients need to know the API function names and arguments
    * Not good for API that needs to be flexible
* REST (Representational state transfer)
    * Client get data from a server by accessing a URI, where the server can return some resource. Uses a data model.
    * Often used for public API's, and is accessed through headers
    * Not good if API with resources that aren't naturally organized in a hierarchy that fits well in headers.
