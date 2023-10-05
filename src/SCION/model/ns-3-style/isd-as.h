#pragma once


#define	IABYTES        8
#define	ISDBits        16
#define	ASBits         48
#define	BGPASBits      32
#define	MaxISD    ISD  (1 << ISDBits) - 1 // uint16_t
#define	MaxAS     AS   (1 << ASBits) - 1
#define	MaxBGPAS  AS   (1 << BGPASBits) - 1 // uint64

#define	asPartBits  16
#define	asPartBase  16
#define asPartMask  (1 << asPartBits) - 1
#define	asParts     ASBits / asPartBits
