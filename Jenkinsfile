node {

	catchError {
		stage('CheckOut') {

		   checkout scm

		}
		stage('Build') {
			
			def TWIST_HOME = "${env.JENKINS_HOME}/jobs/twist5/workspace"
			echo "Running ${env.BUILD_ID} on ${env.JENKINS_URL}. Twist home is $TWIST_HOME"
			
			docker.image('amarillion/alleg5-plus-buildenv:latest').
				inside("-v $TWIST_HOME:$TWIST_HOME") {
				
				sh "make test TWIST_HOME=$TWIST_HOME"
				sh "make all TWIST_HOME=$TWIST_HOME"
			}
		}
	}

//	mailIfStatusChanged env.EMAIL_RECIPIENTS
	mailIfStatusChanged "mvaniersel@gmail.com"
}

//see: https://github.com/triologygmbh/jenkinsfile/blob/4b-scripted/Jenkinsfile
def mailIfStatusChanged(String recipients) {
    
	// Also send "back to normal" emails. Mailer seems to check build result, but SUCCESS is not set at this point.
    if (currentBuild.currentResult == 'SUCCESS') {
        currentBuild.result = 'SUCCESS'
    }
    step([$class: 'Mailer', recipients: recipients])
}
