if [[ ${debug_level} = "1" ]]
then
    sudo ./dnsrelay -d $net_ip
elif [[ ${debug_level} = "2" ]]
then
    sudo ./dnsrelay -dd $net_ip
else
    sudo ./dnsrelay $net_ip
fi
