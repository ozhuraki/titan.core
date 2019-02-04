# Titan contributor workflow  
(Committing directly to eclipse/titan.* using Gerrithub)
Step-by-step guide for contributors

## Prerequisites
* Working git setup
* Registration with GitHub
* Registration with Eclipse (don’t forget to add your GitHub ID to the registration form) and signed ECA
* Full Committer status on the tools.titan project
* GerritHub login tested and working with GitHub ID
* Add Your ssh public key both to GitHub and GerritHub accounts

## Clone the GerritHub repo and make the change
* Clone the repo from GerritHub
    * git clone ssh://<username>@review.gerrithub.io:29418/eclipse/titan.core
    * git clone ssh://<username>@review.gerrithub.io:29418/eclipse/titan.EclipsePlug-ins

* Modify the files and compile

## Test the changes

* For every change tests should be added;the test code should be committed together with the modified  source code 
* If any existing tests, those should be executed before committing the code to make sure nothing is broken; for titan.core and the Eclipse plug-ins a comprehensive set of function and regression tests exists; for some protocol modules or test ports these tests might be absent, however some tests for the modifications should be added; if unsure how existing tests should be executed, please ask.
* Later in the Pull Request please include a statement about the successful execution of the tests

## Add the files and commit them

* Pull the eventual changes from the central repo (and resolve merge conflicts if any)
    * git pull git@github.com:eclipse/titan.*
    * git push
* Use “git add” to add changed files to the commit
    * git add <changed_file1>
    * git add <changed_file2>
    * git add <changed_file3>
…
Or:
    * git add -A
* When committing the change to the local repo don’t forget to use the “-s” flag
    * git commit -s -m “CommitMessage”
* Push the change using the following command:
    * git push origin HEAD:refs/for/master

## Add reviewers
* In GerritHub look for the latest change in “My” -> “Changes”
* Add reviewers using the tiny figure button with the plus sign to the right of “Reviewers”


# Simplified Titan contributor workflow
(to be applied when agreed previously)
Step-by-step guide for contributors

## Prerequisites

* Working git setup
* Registration with GitHub (don’t forget to add your ssh public key!)
* Registration with Eclipse (don’t forget to add your GitHub ID to the registration form) and signed ECA

## Create your own Fork on GitHub

* Log in to GitHub
* Search for the repository you are interested in, say “titan.misc”
* Click on “eclipse/titan.misc” (should be the first in the result list)
* Click on “Fork” button on the top right
* You should be redirected to <username>/titan.misc repository

## Clone Your GitHub repo, make the changes and compile

* Clone the repo
    * git clone git@github.com:/<username>/titan.misc.git
Or
    * git clone https://github.com/<username>/titan.misc.git
* Add your changes and compile 

## Test the changes

* For every change tests should be added; the test code should be committed together with the modified  source code 
* If any existing tests, those should be executed before committing the code to make sure nothing is broken; for titan.core and the Eclipse plug-ins a comprehensive set of function and regression tests exists; for some protocol modules or test ports these tests might be absent, however some tests for the modifications should be added; if unsure how existing tests should be executed, please ask.
* Later in the Pull Request please include a statement about the successful execution of the tests

## Add your changes and commit

* Use “git add” to add changed files to the commit:
    * git add <changed_file1>
    * git add <changed_file2>
    * git add <changed_file3>
    * ...
Or
    * git add -A
* When committing the changes to the local repo don’t forget to use the “-s” (sign-off) flag
    * git commit -s -m “CommitMessage”
* Push the changes using the following command:
    * git push




