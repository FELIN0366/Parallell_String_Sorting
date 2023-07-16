# 给定一个字符串数组，将所有字符串分组，每一组的字符串包含的字符相同但是顺序不同
# 例如：输入：["eat", "tea", "tan", "ate", "nat", "bat"]
# 输出：[["ate", "eat", "tea"], ["nat", "tan"], ["bat"]]
def groupAnagrams(strs):
    """
    :type strs: List[str]
    :rtype: List[List[str]]
    """
    # 创建一个字典，key为排序后的字符串，value为排序后的字符串对应的字符串列表
    dic = {}
    for s in strs:
        # 将字符串排序
        s_sorted = ''.join(sorted(s))
        # 如果字典中没有这个key，就创建这个key，并且将这个key的value设置为一个空列表
        if s_sorted not in dic:
            dic[s_sorted] = []
        # 将字符串添加到key对应的列表中
        dic[s_sorted].append(s)
    # 将字典中的value转换为列表
    return list(dic.values())

if __name__ == '__main__':
    # 从./data.txt中读取数据
    with open('./559649.txt', 'r') as f:
        data = f.read()
    # 将字符串转换为列表
    data = data.split('\n')
    # 调用函数
    res = groupAnagrams(data)
    # 将res列表重排序，具有更多个元素的列表排在前面
    res.sort(key=lambda x: len(x), reverse=True)
    print(res)
    # 统计各种长度的列表的个数
    dic = {}
    for r in res:
        if len(r)==1:
            print('len=1的字符为:'+str(r))
        if len(r) not in dic:
            dic[len(r)] = 0
        dic[len(r)] += 1

    print(dic)
    print(sum(dic.values()))
    print(sum([k * v for k, v in dic.items()]))