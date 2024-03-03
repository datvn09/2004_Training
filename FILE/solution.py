def iter_reccur(n):
    if not n:
        return 1
    if n == 1:
        return 3
    reccur_vals = [1, 3]
    for i in range(2, n + 1):
        val = 2 * reccur_vals[i - 1] + 3 * reccur_vals[i - 2]
        reccur_vals.append(val)
    return reccur_vals[n]

enc_flag = [102, 75, 163, 239, 156, 158, 7, 143, 92, 120, 0, 54, 183, 65, 199, 253, 60, 182, 204]
flag = ""
for i in range(19):  
    flag_val = enc_flag[i]
    ctr_val = iter_reccur((i * i) + 1) % 256
    val = flag_val ^ ctr_val
    flag += chr(val)

print(flag)