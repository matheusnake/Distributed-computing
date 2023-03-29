import findspark
findspark.init()

from time import sleep
from pyspark import SparkContext
from pyspark.streaming import StreamingContext

sc = SparkContext("local[2]", "TP2-Streaming")
ssc = StreamingContext(sc, 120)

lines_RDD = ssc.socketTextStream("localhost", 12345)

genre = lines_RDD.map( lambda text: text.split(","))\
                .filter( lambda word: word[2])\
                .map(lambda word: (word[2], 1))\
                .reduceByKey( lambda a, b: a + b )

genre.saveAsTextFiles("Documents\Distribuidos\Pyspark\Result")

ssc.start()
ssc.awaitTermination() 