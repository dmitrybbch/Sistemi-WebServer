<html>
<h1>Simple webserver</h1>
<p>Simple webserver for a University project.</p>
<hr>

<h2>Example</h2>
<p>Compile with</p>
<pre>gcc serversjf.c lib/concurrentMinheap.c lib/thpool.c -D THPOOL_DEBUG -pthread -lpthread -pg -lm -static -o serversjf</pre>
<p>You run the server like this:</p>
<pre>./server [numberOfThreads] [port]</pre>
<p>For example, to run it on port 8080, you'd do something like this:</p>
<pre>./server 4 10000</pre>
</html>
