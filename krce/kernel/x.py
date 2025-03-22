from pwn import *

context.terminal = ["tmux","new-window"]


def allocate(r,index,size):
    r.sendline(b"1")
    r.recvuntil(b"index:")
    r.sendline(str(index).encode())
    r.recvuntil(b"size:")
    r.sendline(str(size).encode())

def edit(r,index,size,data):
    r.sendline(b"2")
    r.recvuntil(b"index:")
    r.sendline(str(index).encode)
    r.recvuntil(b"size:")
    r.sendline(str(size).encode())
    r.recvuntil(b"data:")
    r.sendline(data)

def show(r,index,size):
    r.sendline(b"3")
    r.recvuntil(b"index:")
    r.sendline(str(index).encode())
    r.recvuntil(b"size:")
    r.sendline(str(size).encode())
    r.recvuntil(b"Data:")
    data = r.recvuntil(b">")[:-2]
    return data

def delete(r,index):
    r.sendline(b"4")
    r.sendline(str(index).encode())

def main():
    r = process("./run.sh")
    log.warn("booting...")
    r.recvuntil(b"[ kRCE - zer0pts CTF 2022 ]")

    

    r.interactive()

if __name__ == "__main__":
    main()