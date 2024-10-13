# 生成公钥
ssh-keygen
# 复制公钥到远程主机
ssh-copy-id -i ~/.ssh/id_rsa.pub -p 8022 u0_a463@192.168.0.108
# 验证
ssh -p 8022 u0_a463@192.168.0.108
