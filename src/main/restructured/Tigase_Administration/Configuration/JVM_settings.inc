JVM settings and recommendations
------------------------------------

Tigase configuration file ``tigase.conf`` (described in more detail in `??? <#manualconfig>`__) mentioned a couple of environmental variables which are related to the operation of the JVM. In this guide we would like to expound on those configuration options and provide hints for the optimal settings.

Settings included in the ``etc/tigase.conf`` are as follows:

.. code:: bash

   #GC="-XX:+UseBiasedLocking -XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:NewRatio=2 -XX:+CMSIncrementalMode -XX:-ReduceInitialCardMarks -XX:CMSInitiatingOccupancyFraction=70 -XX:+UseCMSInitiatingOccupancyOnly"
   #EX="-XX:+OptimizeStringConcat -XX:+DoEscapeAnalysis -XX:+UseNUMA"

   #GC_DEBUG=" -XX:+PrintTenuringDistribution -XX:+PrintGCDetails -XX:+PrintGCDateStamps -XX:+PrintGCTimeStamps -Xloggc:logs/jvm.log -verbose:gc "

   #PRODUCTION_HEAP_SETTINGS=" -Xms5G -Xmx5G " # heap memory settings must be adjusted on per deployment-base!
   JAVA_OPTIONS="${GC} ${GC_DEBUG} ${EX} ${ENC} ${DRV} ${JMX_REMOTE_IP} -server ${PRODUCTION_HEAP_SETTINGS} ${DNS_RESOLVER} ${INTERNAL_IP} ${EXTERNAL_IP}  -XX:MaxDirectMemorySize=128m "

And while this file utilizes bash variables, JVM configuration options can be used in the same manner on all operating systems.

The guide will consists of two main parts - memory settings and Garbage Collector tweaks descriptions and hints.

We recommend using ``-server`` JVM parameter in all cases.

Heap Sizing
^^^^^^^^^^^^^^^^

For the non-production deployments (development or stating environments) we recommend using default memory settings of the JVM (which depends on the underlaying operating system), which result i automatic memory allocation and, by the rule of thumb - are the safest in such environments.

For the production environments we recommend a fixed size HEAP - both initial and maximum size, which can be set with (respectively)``-Xms`` and ``-Xmx`` JVM flags - ideally to the same value (which should be roughly 95% of the available memory, if Tigase will be the only service on the machine) to avoid allocation and deallocation.

For convenience it’s possible to uncomment line with ``PRODUCTION_HEAP_SETTINGS`` and adjust parameters accordingly.

Memory consideration - total usage
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The HEAP size is not the only thing that affects JVM memory usage. When trying to size accordingly for your usage and machine specification you have to consider other factors that count towards total: loaded classes, threads' stack, JIT code cache, garbage collector and others. In principle consider following equation:

::

   Maximum memory usage = [-Xmx] + [-XX:MaxMetaspaceSize] + number_of_threads * [-Xss] + [-XX:MaxDirectMemorySize]
                          (heap)   (classes)                (threads' stack)             (direct memory)


.. **Note**::

   before Java8 memory dedicated to loaded classes was configured with ``-XX:PermSize`` and ``-XX:MaxPermSize`` instead of, respectively, ``-XX:MetaspaceSize`` and ``-XX:MaxMetaspaceSize``

In case of Tigase XMPP Server, apart from heap we limit remaining factors:

-  direct memory to **128** MB

-  loaded classes to **128** MB

-  single thread’s stack size to **228** KB (number of threads depends on number of CPU cores and may vary from 500 to couple of thousands)

In principle, in addition to HEAP’s maximum size defined by ``-Xmx`` you should add roughly **512** MB

If you are interested in detailed tracking of memory take a look at [Memory footprint of the JVM](\ https://spring.io/blog/2019/03/11/memory-footprint-of-the-jvm/), [Native Memory Tracking in JVM](\ https://www.baeldung.com/native-memory-tracking-in-jvm) or [Why does my Java process consume more memory than Xmx?](\ https://plumbr.io/blog/memory-leaks/why-does-my-java-process-consume-more-memory-than-xmx)


GC settings
^^^^^^^^^^^^^^

Let’s start with stating that there is no "one to rule them all" - each deployment and use-case is different, however we will try to give a couple of pointers and recommendations proceed with short introduction to GC itself.

XMPP is quite specific in terms of memory allocation - short-lived objects (various types of stanzas) usually exceed number of long-lived objects (user connections and related data). This is important bit of information in the context of how usually JVM HEAP is organized and how Garbage Collector works. On the most basic level Heap is separated into couple of regions:

Generations
~~~~~~~~~~~~~~

-  **Young Generation**, which is further divided in to:

   -  **Eden** - the region when the objects are usually allocated when they are created;

   -  **Survivor Spaces** - (*to* and *from* - one of which is always empty) - responsible for storing all live object remaining after collecting **Young Generation** (process is repeated several times until objects are finally considered *old enough*);

-  **Old Generation** - (*Tenured Space*) - responsible for live objects remaining after running GC on **Survivor Spaces** - those would be *long-lived* objects (usually user connections and associated data);

Minor, Major and Full GC - optimizing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

General thinking suggests that:

-  **Minor GC** cleans Young generation;

-  **Major GC** cleans Tenured space;

-  **Full GC** cleans all heap.

However, while we can certainly state that Minor GC cleans Young generation it’s a bit more difficult to differentiate Major and Full GC, especially considering that Major GC can be quite often triggered by Minor GC and some garbage collectors can perform cleaning concurrently. Instead of focusing of distinguishing phases one should pay closer attention to actual operations of Garbage Collector itself - uncommenting the line ``GC_DEBUG=" -XX:+PrintTenuringDistribution -XX:+PrintGCDetails -XX:+PrintGCDateStamps -XX:+PrintGCTimeStamps -Xloggc:logs/jvm.log -verbose:gc "`` in ``etc/tigase.conf`` (or adding same properties to the java commandline) and subsequently analyzing the results should prove more helpful. In addition monitoring GC operation using for example VisualVM (with VisualGC plugin) will also be helpful.

Settings for XMPP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ideally we should limit both number of GC pauses as well as their duration. After running rather tests following conclusions were made:

-  Garbage Collection is the faster the more dead objects occupies given space, therefore on high-traffic installation it’s better to have rather large YoungGen resulting in lower promotion of the objects to the OldGen;

-  with JVM8 default sizing of Young / Old generation changed, even tho NewRatio is still defaulting to “2” - setting it explicitly to "2" brought back previous sizing;

-  Concurrent Mark and Sweep (CMS) enabled (applies to Tenured space only) with explicit configuration of NewRatio set to default value of 2 (i.e. ``-XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:NewRatio=2``) in general behaves best;

-  For small installations (few core CPU, less memory) with low traffic default Parallel collector may be a better solution;

-  Using Heap size adjusted to the actual usage is better as the larger the heap the larger are spaces over which collection needs to be performed thus resulting in longer pauses; in case of huge heaps G1 collector may be better solution to avoid longer pauses;

Considering all of the above using following options should be a good starting point toward further optimizing of Garbage Collection:

``GC="-XX:+UseBiasedLocking -XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:+CMSIncrementalMode -XX:-ReduceInitialCardMarks -XX:CMSInitiatingOccupancyFraction=70 -XX:+UseCMSInitiatingOccupancyOnly"``

GC settings worth considering
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In addition to the general recommendation to use CMS collector, following options (or changes to the options) may be worth considering:

-  ``-XX:NewRatio=2`` - defines the ratio between the young and tenured generation is 1:2. In other words, the combined size of the eden and survivor spaces will be one-third of the total heap size. The parameters NewSize and MaxNewSize bound the young generation size from below and above. Setting these to the same value fixes the young generation, just as setting -Xms and -Xmx to the same value fixes the total heap size.

-  ``-XX:CMSInitiatingOccupancyFraction=percent`` - sets the percentage of the old generation occupancy (0 to 100) at which to start a CMS collection cycle.

-  ``-XX:+UseCMSInitiatingOccupancyOnly`` - instructs the JVM not to base its decision when to start a CMS cycle on run time statistics but instead it uses the value of CMSInitiatingOccupancyFraction for every CMS cycle.

-  ``-XX:ParallelGCThreads=x`` - sets the number of threads used for parallel garbage collection in the young and old generations. The default value depends on the number of CPUs available to the JVM. If the Tigase JMV is the only one running on the installation default value is recommended.

-  ``-XX:ConcGCThreads=x`` - sets the number of threads used for concurrent GC. The default value depends on the number of CPUs available to the JVM. If the Tigase JMV is the only one running on the installation default value is recommended.

-  ``-XX:+UseBiasedLocking`` and ``-XX:+DoEscapeAnalysis`` - designed to eliminate locking overhead, however their effect on performance is unpredictable therefore testing is required; reduced locking should improve concurrency and, on current multi-core hardware, improve throughput.

-  ``-XX:+OptimizeStringConcat`` - enables the optimization of String concatenation operations. This option is enabled by default.

-  ``-XX:+UseNUMA`` - enables performance optimization of an application on a machine with nonuniform memory architecture (NUMA - most modern computers are based on NUMA architecture) by increasing the application’s use of lower latency memory. By default, this option is disabled and no optimization for NUMA is made. The option is only available when the parallel garbage collector is used (-XX:+UseParallelGC).

-  ``-XX:-UseCompressedOops`` — disables the use of compressed pointers. By default, this option is enabled, and compressed pointers are used when Java heap sizes are less than 32 GB. When this option is enabled, object references are represented as 32-bit offsets instead of 64-bit pointers, which typically increases performance when running the application with Java heap sizes less than 32 GB. This option works only for 64-bit JVMs.


What to use with Machine x, y, z?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Server class machine (non-VM), > 16GB, >= 8 core CPU
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For such setup enabling CMS garbage collector is recommended. Depending on the traffic usage and particular use-case adjusting NewRatio may be needed. Adjusting Xms and Xms sizes for actual available memory is needed (or better yet, for the actual traffic!). Following should be used:

.. code:: bash

   GC="-XX:+UseBiasedLocking -XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:NewRatio=2 -XX:+CMSIncrementalMode -XX:-ReduceInitialCardMarks -XX:CMSInitiatingOccupancyFraction=70 -XX:+UseCMSInitiatingOccupancyOnly"
   EX="-XX:+OptimizeStringConcat -XX:+DoEscapeAnalysis -XX:+UseNUMA"

   #GC_DEBUG=" -XX:+PrintTenuringDistribution -XX:+PrintGCDetails -XX:+PrintGCDateStamps -XX:+PrintGCTimeStamps -Xloggc:logs/jvm.log -verbose:gc "

   PRODUCTION_HEAP_SETTINGS=" -Xms15G -Xmx15G " # heap memory settings must be adjusted on per deployment-base!
   JAVA_OPTIONS="${GC} ${GC_DEBUG} ${EX} ${ENC} ${DRV} ${JMX_REMOTE_IP} -server ${PRODUCTION_HEAP_SETTINGS} ${DNS_RESOLVER} ${INTERNAL_IP} ${EXTERNAL_IP}  -XX:MaxDirectMemorySize=128m "

For installation with lot of available memory and intention to utilize it all, using G1GC collector may be a better idea :

.. code:: bash

   GC="-XX:+UseG1GC -XX:ConcGCThreads=4 -XX:G1HeapRegionSize=2 -XX:InitiatingHeapOccupancyPercent=35 -XX:MaxGCPauseMillis=100"
   EX="-XX:+OptimizeStringConcat -XX:+DoEscapeAnalysis -XX:+UseNUMA"

   #GC_DEBUG=" -XX:+PrintTenuringDistribution -XX:+PrintGCDetails -XX:+PrintGCDateStamps -XX:+PrintGCTimeStamps -Xloggc:logs/jvm.log -verbose:gc "

   PRODUCTION_HEAP_SETTINGS=" -Xms60G -Xmx60G " # heap memory settings must be adjusted on per deployment-base!
   JAVA_OPTIONS="${GC} ${GC_DEBUG} ${EX} ${ENC} ${DRV} ${JMX_REMOTE_IP} -server ${PRODUCTION_HEAP_SETTINGS} ${DNS_RESOLVER} ${INTERNAL_IP} ${EXTERNAL_IP}  -XX:MaxDirectMemorySize=128m "


VM machine, 8GB of RAM, 4 core CPU equivalent
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For such setup enabling CMS garbage collector is also recommended. Depending on the traffic usage and particular use-case adjusting NewRatio may be needed (and configuring NewRatio is a must!). Adjusting Xms and Xms sizes for actual available memory is needed (or better yet, for the actual traffic!). Following should be used:

.. code:: bash

   GC="-XX:+UseBiasedLocking -XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:NewRatio=2 -XX:+CMSIncrementalMode -XX:-ReduceInitialCardMarks -XX:CMSInitiatingOccupancyFraction=70 -XX:+UseCMSInitiatingOccupancyOnly"
   EX="-XX:+OptimizeStringConcat -XX:+DoEscapeAnalysis -XX:+UseNUMA"

   #GC_DEBUG=" -XX:+PrintTenuringDistribution -XX:+PrintGCDetails -XX:+PrintGCDateStamps -XX:+PrintGCTimeStamps -Xloggc:logs/jvm.log -verbose:gc "

   PRODUCTION_HEAP_SETTINGS=" -Xms7G -Xmx7G " # heap memory settings must be adjusted on per deployment-base!
   JAVA_OPTIONS="${GC} ${GC_DEBUG} ${EX} ${ENC} ${DRV} ${JMX_REMOTE_IP} -server ${PRODUCTION_HEAP_SETTINGS} ${DNS_RESOLVER} ${INTERNAL_IP} ${EXTERNAL_IP}  -XX:MaxDirectMemorySize=128m "


VM machine with 4GB or less of RAM, and less than 4 core CPU equivalent
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Small installations with limited resources could operate better with default (for JVM versions up to 8, which is the most current at the moment of the writing). Again - depending on the traffic usage and particular use-case adjusting NewRatio may be needed. Adjusting Xms and Xms sizes for actual available memory is recommended (or better yet, for the actual traffic!). Following should be used (i.e. ``GC`` line should be commented so the defaults will be used):

.. code:: bash

   #GC="-XX:+UseBiasedLocking -XX:+UseConcMarkSweepGC -XX:+UseParNewGC -XX:NewRatio=2 -XX:+CMSIncrementalMode -XX:-ReduceInitialCardMarks -XX:CMSInitiatingOccupancyFraction=70 -XX:+UseCMSInitiatingOccupancyOnly"
   EX="-XX:+OptimizeStringConcat -XX:+DoEscapeAnalysis -XX:+UseNUMA"

   #GC_DEBUG=" -XX:+PrintTenuringDistribution -XX:+PrintGCDetails -XX:+PrintGCDateStamps -XX:+PrintGCTimeStamps -Xloggc:logs/jvm.log -verbose:gc "

   PRODUCTION_HEAP_SETTINGS=" -Xms3G -Xmx3G " # heap memory settings must be adjusted on per deployment-base!
   JAVA_OPTIONS="${GC} ${GC_DEBUG} ${EX} ${ENC} ${DRV} ${JMX_REMOTE_IP} -server ${PRODUCTION_HEAP_SETTINGS} ${DNS_RESOLVER} ${INTERNAL_IP} ${EXTERNAL_IP}  -XX:MaxDirectMemorySize=128m "


Additional resources
^^^^^^^^^^^^^^^^^^^^^^^^

-  `Sizing the Generations <https://docs.oracle.com/javase/8/docs/technotes/guides/vm/gctuning/sizing.html>`__

-  `About Java, parallel garbage collection and processor sets <http://www.c0t0d0s0.org/archives/6617-About-Java,-parallel-garbage-collection-and-processor-sets.html>`__

-  `GC Threads <http://hiroshiyamauchi.blogspot.cl/2009/12/gc-threads.html>`__

-  `GCViewer readme <https://github.com/chewiebug/GCViewer#readme>`__

-  `Java HotSpot™ Virtual Machine Performance Enhancements <http://docs.oracle.com/javase/7/docs/technotes/guides/vm/performance-enhancements-7.html>`__

-  `Java Garbage Collection handbook <https://plumbr.eu/java-garbage-collection-handbook>`__

-  Useful JVM Flags

   -  `Part 1 - JVM Types and Compiler Modes <https://blog.codecentric.de/en/2012/07/useful-jvm-flags-part-1-jvm-types-and-compiler-modes/>`__

   -  `Part 2 - Flag Categories and JIT Compiler Diagnostics) <https://blog.codecentric.de/en/2012/07/useful-jvm-flags-part-2-flag-categories-and-jit-compiler-diagnostics/>`__

   -  `Part 3 - Printing all XX Flags and their Values <https://blog.codecentric.de/en/2012/07/useful-jvm-flags-part-3-printing-all-xx-flags-and-their-values/>`__

   -  `Part 4 - Heap Tuning <https://blog.codecentric.de/en/2012/07/useful-jvm-flags-part-4-heap-tuning/>`__

   -  `Part 5 - Young Generation Garbage Collection <https://blog.codecentric.de/en/2012/08/useful-jvm-flags-part-5-young-generation-garbage-collection/>`__

   -  `Part 6 - Throughput Collector <https://blog.codecentric.de/en/2013/01/useful-jvm-flags-part-6-throughput-collector/>`__

   -  `Part 7 - CMS Collector <https://blog.codecentric.de/en/2013/10/useful-jvm-flags-part-7-cms-collector/>`__

   -  `Part 8 - GC Logging <https://blog.codecentric.de/en/2014/01/useful-jvm-flags-part-8-gc-logging/>`__

