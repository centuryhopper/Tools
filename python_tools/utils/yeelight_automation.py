#! /home/leo_zhang/miniconda3/envs/web_auto/bin/python

from yeelight import Bulb, discover_bulbs

bulbs = discover_bulbs()
print(bulbs)

bulb = Bulb(bulbs[0]['ip'])


bulb.turn_off()


