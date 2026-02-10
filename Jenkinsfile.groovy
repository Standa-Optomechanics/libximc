def ciBuildingTag = false
script {
  if (env.TAG_NAME) {
    ciBuildingTag = true
  }
}

pipeline {
  agent { label "master" }

  parameters {
    booleanParam(name: 'DEBUG', defaultValue: false, description: 'Build with debug symbols')
    booleanParam(name: 'WITH_ARM', defaultValue: "${ciBuildingTag}", description: 'Enable build for ARM')
  }

  triggers {
    // enable weekly rebuilds
    cron('H H(6-9) * * 1')
  }

  options {
    disableConcurrentBuilds()
    parallelsAlwaysFailFast()
    timeout(time: 2, unit: 'HOURS')
  }

  environment {
    DEBUG = "${params.DEBUG}"
    SKIP_CLEAN_CHECKOUT = "yes"
    LIBRARY_VERSION = "${readFile("version").readLines()[0]}"
  }

  stages {

    stage('init') {
      // execute at master
      steps {
        echo "Libximc init"
        echo "Param WITH_ARM=${params.WITH_ARM}"
        echo "Param DEBUG=${params.DEBUG}"
        echo "Library version is ${env.LIBRARY_VERSION}"
      }
    } // stage

    stage('build') {
      matrix {
        // limit execution at agent with matching label
        agent {
          label "${env.BUILDOS}"
        }
        axes {
          axis {
            name 'BUILDOS'
            values 'debian32', 'osx', 'win' //'debian64', 'debian32', 'debianarm', 'suse64', 'suse32',
          }
        }
        stages {
          // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          // $                                     $
          // $          BUILD FOR WINDOWS          $
          // $                                     $
          // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          stage('Build-Win') {
            when {
              allOf {
                expression { !isUnix() };
                // Avoid building tags on cron
                not { allOf { triggeredBy 'TimerTrigger'; buildingTag() } }
              }
            }
            stages {
              // /********************************************\
              // *               PREBUILD STAGE               *
              // \********************************************/
              stage('prebuild-Win') {
                steps {
                  echo "Building at BUILDOS=${BUILDOS}"
                  bat "build.bat clean"
                }
              }
            
              // /********************************************\
              // *          BUILD DEPENDENCIES STAGE          *
              // \********************************************/
              stage('build-dependencies-Win') {
                steps {
                  bat "build.bat deps"
                }
              }

              // /********************************************\
              // *      GENERATE LIBXIMC's SOURCES STAGE      *
              // \********************************************/
              stage('generate-libximc-sources-Win') {
                steps {
                    bat "build.bat gen-libximc-sources"
                }
              }

              // /********************************************\
              // *             BUILD LIBXIMC STAGE            *
              // \********************************************/
              stage('build-libximc-Win') {
                steps {
                  bat "build.bat libximc"
                }
              }

              // /********************************************\
              // *            BUILD WRAPPERS STAGE            *
              // \********************************************/
              stage('build-wrappers-Win') {
                steps {
                    bat "build.bat wrappers"
                }
              }

              // /********************************************\
              // *            BUILD EXAMPLES STAGE            *
              // \********************************************/
              stage('build-examples-Win') {
                steps {
                  bat "build.bat examples"
                }
              }
            }
          } // stage Build-Win
          
          // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          // $                                     $
          // $        BUILD FOR UNIX SYSTEMS       $
          // $                                     $
          // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
          stage('Build-Unix') {
            when {
              allOf {
                expression { isUnix() };
                // Avoid building for ARM in case WITH_ARM is unset
                anyOf {
                  expression { env.BUILDOS != 'debianarm' };
                  expression { params.WITH_ARM }
                };
                // Avoid building tags on cron
                not { allOf { triggeredBy 'TimerTrigger'; buildingTag() } }
              }
            }
            stages {
              // /********************************************\
              // *               PREBUILD STAGE               *
              // \********************************************/
              stage('prebuild-Unix') {
                steps {
                  echo "Building at BUILDOS=${BUILDOS}"
                  sh "./build.sh clean"
		              sh "./build.sh configure"
                }
              }
              
              // /********************************************\
              // *          BUILD DEPENDENCIES STAGE          *
              // \********************************************/
              stage('build-dependencies-Unix') {
                steps {
                  sh "./build.sh deps"
                }
              }

              // /********************************************\
              // *      GENERATE LIBXIMC's SOURCES STAGE      *
              // \********************************************/
              stage('generate-libximc-sources-Unix') {
                steps {
                    sh "./build.sh gen-libximc-sources"
                }
              }

              // /********************************************\
              // *             BUILD LIBXIMC STAGE            *
              // \********************************************/
              stage('build-libximc-Unix') {
                steps {
                  sh "./build.sh libximc"
                }
              }

              // /********************************************\
              // *            BUILD WRAPPERS STAGE            *
              // \********************************************/
              stage('build-wrappers-Unix') {
                steps {
                    sh "./build.sh wrappers"
                }
              }

              // /********************************************\
              // *            BUILD EXAMPLES STAGE            *
              // \********************************************/
              stage('build-examples-Unix') {
                when {
                  expression { env.BUILDOS == 'osx' };
                }
                steps {
                    sh "./build.sh examples"
                }
              }

              // /********************************************\
              // *              GENERATE DOCS STAGE           *
              // \********************************************/
              stage('docs') {
                // There is no need to run this stage on all machines
                // It is enough to run it on one of them
                when {
                  expression { env.BUILDOS != 'debianarm' };
                }
                steps {
                  sh "./build.sh docs"
                }
              } // stage

              // /*******************************************\
              // *              BUILD DEBS STAGE             *
              // \*******************************************/
              stage('debs') {
                when {
                  expression { env.BUILDOS != 'osx' };
                }
                steps {
                  sh "./build.sh debs"
                }
              } // stage
            } // stages
          } // stage Build-Unix

          // /********************************************\
          // *                 STASH STAGE                *
          // \********************************************/
          stage('stash') {
            when {
              allOf {
                // Avoid stashing in case WITH_ARM is unset
                anyOf {
                    expression { env.BUILDOS != 'debianarm' };
                    expression { params.WITH_ARM }
                };
                // Avoid building tags on cron
                not { allOf { triggeredBy 'TimerTrigger'; buildingTag() } }
              }
            }
            steps {
              script {
                if (env.BUILDOS == 'win') {
                  bat "dir"
                  bat "C:/MSYS2/usr/bin/tar -C dist -cf result-%BUILDOS%.tar ximc examples"
                } else if (env.BUILDOS == 'osx' || env.BUILDOS == 'debian32') {
                  sh  "tar -C dist -cf result-${BUILDOS}.tar ximc"
                } else {
                  echo "There is no stashing algorithm for ${env.BUILDOS}! No stash will be created."
                }
              }
              stash name: "result-${BUILDOS}", includes: "result-${BUILDOS}.tar"
            }
          }
        } // stages
        post {
          cleanup {
            // drop workspace for each matrix cell job
            cleanWs(notFailBuild: true)
          }
        }
      } // matrix
    } // stage

    stage('pack') {
      // execute on master
      when {
        // Avoid building tags on cron
        not { allOf { triggeredBy 'TimerTrigger'; buildingTag() } }
      }
      steps {
        // Get all stashed archives
        //unstash "result-debian64"
        unstash "result-debian32"
        unstash "result-win"
        unstash "result-osx"
        script {
          if (params.WITH_ARM) {
            unstash "result-debianarm"
          }
        }
        sh "ls"
        script{
          // 1. Распаковка result-*.tar архивов
          echo "Распаковка архивов result-*.tar"
          sh 'ls -1 result-*.tar | xargs -L1 -t tar xfv'

          // 2. Установка прав доступа и проверка
          echo "Before chmod:"
          sh 'ls -lR ximc'
          echo "Установка прав доступа ximc: u+rX"
          sh 'chmod -R u+rX ximc'
          echo "After chmod:"
          sh 'ls -lR ximc'

          // 3. Распаковка DEB-пакетов
          def architectures = ['amd64', 'i386', 'armhf']

          for (String arch : architectures) {
              echo "Обработка архитектуры: ${arch}"

              // Поиск файлов DEB-пакетов
              def namearch = sh(script: "find ximc/deb -name \"libximc7_*_${arch}.deb\"", returnStdout: true).trim()
              def namearch_dev = sh(script: "find ximc/deb -name \"libximc7-dev_*_${arch}.deb\"", returnStdout: true).trim()

              // Проверка, найден ли файл
              if (!namearch.isEmpty() && fileExists(namearch)) {
                  echo "Найден DEB-архив: ${namearch}"

                  // Создание директорий
                  sh "mkdir -p ximc/deb/${arch}"
                  sh "mkdir -p ximc/deb/dev-${arch}"
                  sh "mkdir -p ximc/debian-${arch}"

                  // Распаковка обычного DEB-пакета
                  sh "ar -x ${namearch} data.tar.gz"
                  sh "mv -f data.tar.gz ximc/deb/${arch}"
                  sh "tar -C ximc/deb/${arch}/ -xf ximc/deb/${arch}/data.tar.gz"

                  // Распаковка dev-DEB-пакета
                  sh "ar -x ${namearch_dev} data.tar.gz"
                  sh "mv -f data.tar.gz ximc/deb/dev-${arch}"
                  sh "tar -C ximc/deb/dev-${arch}/ -xf ximc/deb/dev-${arch}/data.tar.gz"

                  // Копирование библиотек
                  sh "cp -R ximc/deb/${arch}/usr/lib/*.* ximc/debian-${arch}/"
                  sh "cp -R ximc/deb/dev-${arch}/usr/lib/*.* ximc/debian-${arch}/"
                  
                  // Очистка временных директорий
                  sh "rm -rf ximc/deb/${arch}"
                  sh "rm -rf ximc/deb/dev-${arch}"
              } else {
                  echo "No archive file found for architecture: ${arch}"
              }
          } // for architectures
        }
        sh "./jenkins_packer.sh"
        archiveArtifacts artifacts: "dist/libximc*.tar.gz, dist/*-changelog.txt, dist/libximc_bindings_python-*.7z"
      }
    } // stage

  } // stages

  post {
    failure {
      echo "Failure, sending emails..."
      emailext body: '$DEFAULT_CONTENT',
               to: '$DEFAULT_RECIPIENTS',
               recipientProviders: [[$class: 'DevelopersRecipientProvider'],[$class: 'CulpritsRecipientProvider']],
               subject: '$DEFAULT_SUBJECT'
    }
    aborted {
      echo "Aborted, sending emails..."
      emailext body: '$DEFAULT_CONTENT',
               to: '$DEFAULT_RECIPIENTS',
               recipientProviders: [[$class: 'DevelopersRecipientProvider'],[$class: 'CulpritsRecipientProvider']],
               subject: '$DEFAULT_SUBJECT'
    }
    cleanup {
      // drop workspace for main job
      cleanWs(notFailBuild: true)
    }
  }
}

