port=8022
address=u0_a463@192.168.0.108
while true; do
    clear
    date
    for file in $@
    do
        echo "Translating $file..."
        scp -P $port ../$file $address:~
    done
    sleep 2
    
done