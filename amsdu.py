def calculate_amsdu_size(packages, subframe_format, qos_ctrl=True, add_addr=False, ht_ctrl=True):
    mac_header = 2 + qos_ctrl * 2 + 18 + add_addr * 6 + 2 + ht_ctrl * 4 + 2
    
    if subframe_format == 'Basic':
        subframe_prefix = 14
    elif subframe_format == 'Mesh':
        subframe_prefix = 32
    elif subframe_format == 'Short' or subframe_format == 'Dynamic':
        subframe_prefix = 2
    else:
        print('Unknown subframe format\n')
        return

    amsdu = 0
    for i in range(len(packages) - 1):
        amsdu += ((subframe_prefix + packages[i] - 1) // 4 + 1) * 4     # sum of padded subframes sizes except last
    amsdu += (subframe_prefix + packages[-1])                           # adding last subframe size
    return mac_header + amsdu + 4                                   

