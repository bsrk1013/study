rsync -rav --delete -e "ssh -i ~/LightsailDefaultKey-ap-northeast-2.pem" . --include-from=deploy/include.txt --exclude="*" centos@15.164.142.183:/home/centos/webtoonedu-server/
rsync -rav --delete -e "ssh -i ~/LightsailDefaultKey-ap-northeast-2.pem" --rsync-path="mkdir -p /home/centos/webtoonedu-server/server/src && rsync" server/build-server/* centos@15.164.142.183:/home/centos/webtoonedu-server/server/src
rsync -rave "ssh -i ~/LightsailDefaultKey-ap-northeast-2.pem" server/config.production.yml centos@15.164.142.183:/home/centos/webtoonedu-server/server/config.yml
ssh -i ~/LightsailDefaultKey-ap-northeast-2.pem centos@15.164.142.183 'cd /home/centos/webtoonedu-server && npm i'
ssh -i ~/LightsailDefaultKey-ap-northeast-2.pem centos@15.164.142.183 'cd /home/centos/webtoonedu-server && bash bin/stop.sh'
ssh -i ~/LightsailDefaultKey-ap-northeast-2.pem centos@15.164.142.183 'cd /home/centos/webtoonedu-server && bash bin/start.sh'