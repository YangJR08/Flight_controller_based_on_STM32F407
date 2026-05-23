
## Git指令解析
1、`git remote add origin git@github.com:YangJR08/Flight_controller_based_on_STM32F407.git`
`remote`操作「远程仓库」（就是 GitHub 上的仓库）
`add`添加 / 建立关联
`origin`给远程仓库起的默认别名（约定俗成的名字，代表「主远程仓库」，不用改）
`git@github.com:YangJR08/Flight_controller_based_on_STM32F407.git`GitHub 仓库的 SSH 地址（远程仓库的唯一地址）
2、`git branch -M main`
`git branch`Git 专门用来操作分支的命令（查看、创建、重命名分支都用它）
`-M`大写的 M，是 --move --force 的缩写，意思是：强制重命名分支
小写 -m：普通重命名（重名会报错）
大写 -M：强制重命名（不管有没有重名，直接改）
`main`新的分支名字（现在 GitHub / 码云 统一用 main 作为默认主分支）
2、`git push -u origin main`
`git push`Git 核心命令：把本地仓库的代码，推送到远程仓库（GitHub）
`-u`全称`--set-upstream`，绑定关联作用：让本地`main`分支和远程`main`分支永久绑定这是第一次推送必须加的参数
`origin`之前用`git remote add`设置的远程仓库别名（代表 GitHub 上的飞控仓库）
`main`要上传的本地分支名（就是刚重命名的主分支）
## 串口输出打印
使用串口1
CubeMX中配置串口1
`User label` P_TX P_RX
添加公共层`Commom`，并在里面添加`Com_debug.c`完成`int fputc(int ch, FILE *f)`重定向
