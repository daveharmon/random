import tensorflow.contrib.learn as skflow
from sklearn import datasets, metrics

# 	collect data
#	datasets come in all kinds of formats, just need to parse it somehow
# 	this sample is labeled, so we are doing supervised learning, if we only
#	had features then it would be unsupervised.  This is a good data set 
#	because each data point is simple, independent, and informative
#
#	deciding on features should go along the same lines as you would in real
#	life
iris = datasets.load_iris()

# 	pick the model
#	since we are trying to classify an image as either an iris flower or not, we
# 	use a classifier.  If we had more data we would want to use something like
#	a deep neural network, but we only have 150 samples so we just use a linear algo.
# 	switching out algorithms can show differences in accuracy, telling you which to use
#
#	This has accuracy .953333
# classifier = skflow.TensorFlowDNNClassifier(hidden_units = [10, 20, 10], n_classes=3)
#	This has accuracy .960000
classifier = skflow.TensorFlowLinearClassifier(n_classes=3)


# 	add data, classifier will find patterns for us
classifier.fit(iris.data, iris.target)

score = metrics.accuracy_score(iris.target, classifier.predict(iris.data))
print 'Accuracy: %f' % score

# 	now when we import new data points of an iris flower, 
# 	it will be able to classify it for us!