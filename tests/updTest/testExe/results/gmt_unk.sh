#!/bin/bash
#
awk '{ if( $1=="WL")print $5, $7,$8,$7-$8}' updzap.50.unk.number  > updzap.wl.50.unk
awk '{ if( $1=="NL")print $5, $7,$8,$7-$8}' updzap.50.unk.number  > updzap.nl.50.unk
awk '{ if( $1=="WL")print $5, $7,$8,$7-$8}' updzap.70.unk  > updzap.wl.70.unk
awk '{ if( $1=="NL")print $5, $7,$8,$7-$8}' updzap.70.unk  > updzap.nl.70.unk
awk '{ if( $1=="WL")print $5, $7,$8,$7-$8}' updzap.90.unk  > updzap.wl.90.unk
awk '{ if( $1=="NL")print $5, $7,$8,$7-$8}' updzap.90.unk  > updzap.nl.90.unk
