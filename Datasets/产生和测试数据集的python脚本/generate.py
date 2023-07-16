import random
import string
from tqdm import tqdm

# 定义字符集
CHAR_SET = string.ascii_lowercase

# 定义生成字符串数量和长度范围
NUM_STRINGS = 100000
MIN_LENGTH = 2
MAX_LENGTH = 3

# 生成随机字符串
strings = []
for i in tqdm(range(NUM_STRINGS)):
    length = random.randint(MIN_LENGTH, MAX_LENGTH)
    string = ''.join(random.sample(CHAR_SET, length))
    strings.append(string)

# 将字符串顺序打乱
for i in tqdm(range(NUM_STRINGS)):
    strings[i] = ''.join(random.sample(strings[i], len(strings[i])))

# 对strings去重
strings = list(set(strings))

# 保存到文件
with open('data.txt', 'w') as f:
    for string in strings:
        f.write(string + '\n')
