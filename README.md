<html>
<h1>SO Webserver</h1>
<p>Small webserver for a University project. It has two versions: a normal FIFO version, and an SJF version, implemented with a minheap to keep track of the smallest size file requested. "</p>
<hr>

<h2>Example</h2>
<p>Compile with</p>
<pre>gcc serversjf.c lib/concurrentMinheap.c lib/thpool.c -D THPOOL_DEBUG -pthread -lpthread -pg -lm -static -o serversjf</pre>
<p>You run the server like this:</p>
<pre>./server [numberOfThreads] [port]</pre>
<p>For example, to run it on port 8080, you'd do something like this:</p>
<pre>./server 4 10000</pre>
</html>
