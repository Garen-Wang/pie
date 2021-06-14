# from lstm.loader import Wu_loader

import numpy as np

import torch

import torch.nn as nn

import torch.optim as optim

import random

import matplotlib.pyplot as plt

from torch.utils.data import DataLoader

import pandas as pd

torch.set_default_tensor_type(torch.FloatTensor)

"""
just a comment
just another comment
"""


def setup_seed(self, seed):
    torch.manual_seed()

    torch.cuda.manual_seed_all(seed)

    np.random.seed(seed)

    random.seed(seed)

    torch.backends.cudnn.deterministic = True


setup_seed(20)


def pro1(t, ma, mi):
    t = (t - mi) / (ma - mi)

    return np.array(t).astype(np.float32)


tar = pd.read_csv('.\hubeisheng.csv')

sq = tar.shape[0] - 1

fea = 3

pa_num = 2  # dead(i-r),trans(e-i,or 1-i),attack(i-m)

bs = 1

ai = tar['day_quezhenrenshu']

aima = ai.max()

aimi = ai.min()

ai = pro1(ai, aima, aimi)

i = tar['illnow']

ima = i.max()

imi = i.min()

i = pro1(i, ima, imi)

m = tar['day_miqiejiechuzherenshu']

mma = m.max()

mmi = m.min()

m = pro1(m, mma, mmi)

e = tar['yixueguancharenshu:zhengzai']

ema = e.max()

emi = e.min()

e = pro1(e, ema, emi)

r = tar['day_zhiyurenshu'] + tar['day_siwangrenshu']

rma = r.max()

rmi = r.min()

r = pro1(r, rma, rmi)

all = np.vstack((i, e, r, ai)).T.reshape(sq + 1, 1, 4)

# label=all.delete(0,aixs=0)

inp = np.vstack((i, e, m)).T.reshape(sq + 1, 1, fea)

inp = np.delete(inp, -1, axis=0)

inp = torch.tensor(inp)

label = np.delete(all, 0, axis=0)

label = torch.tensor(label)


class Lsnet(nn.Module):

    def __init__(self, hl=5, nl=2):
        super(Lsnet, self).__init__()

        self.hl = hl

        self.l1 = nn.LSTM(fea, hl, nl)

        self.l2 = nn.Linear(hl, pa_num)

        self.l3 = nn.Sigmoid()

        self.h0 = torch.randn(nl, 1, hl)

        self.c0 = torch.randn(nl, 1, hl)

    def forward(self, x):
        x, _ = self.l1(x, hx=(self.h0, self.c0))

        s, b, n = x.size()

        x = x.view(s * b, n)

        x = self.l2(x)

        x = x.view(s, b, pa_num)

        x = self.l3(x)

        return x


ls = Lsnet()

opt = optim.Adam(ls.parameters(), lr=0.002, betas=(0.5, 0.999))


def loss(paras):
    sum = 0

    for index in range(sq):
        sum += (label[index][0][2] - paras[index][0][0] * inp[index][0][0]) ** 2

        sum += (label[index][0][3] - (1 * paras[index][0][1])) ** 2

    return sum


def save():
    torch.save(ls.state_dict(), "./station/use1.pt")


def step():
    # index=np.random.uniform(0,100,10)

    opt.zero_grad()

    out = ls(inp)

    lo = loss(out)

    lo.mean().backward()

    opt.step()

    return out, lo.mean()


for i in range(4001):

    out, lo = step()

    if (i % 1000 == 0):
        print(lo)

        # plt.plot(np.arange(1000),label.transpose(0,1).flatten().data.numpy(),'r',label='raw')

        # plt.plot(np.arange(1000),ls(inp).transpose(0,1).flatten().data.numpy(),'b',label='pre')

        plt.subplot(1, 2, 1)

        plt.plot(range(sq), label[:, 0, 2].data, "r", "raw_r")

        plt.show()

        save()
