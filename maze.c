void getDir(unsigned char directions, unsigned int *east,unsigned int *north,unsigned int *west,unsigned int *south)
{
    // EE NN WW SS >> 6 = 00 00 00 EE(shifts the whole number 6 bits to the right)
    // 00 00 00 EE & 00 00 00 11 = EE(uses a mask to only look at the last 2 bits)
    *east = (directions >> 6) & 0b00000011;
    *north = (directions >> 4) & 0b00000011;
    *west = (directions >> 2) & 0b00000011;
    *south = directions & 0b00000011;
}

int setDir(unsigned char *directions,unsigned int east,unsigned int north,unsigned int west,unsigned int south)
{
    if((east > 2 || north > 2) || (west > 2 || south > 2))
        return -1;
    else
    {
        //east = EE     north = NN      west = WW       south = SS
        //east << 6     = EE 00 00 00
        //north << 4    = 00 NN 00 00
        //west << 2     = 00 00 WW 00
        //south         = 00 00 00 SS

        //(east << 6) | (north << 4) = EE NN 00 00
        // etc...
        *directions = (east << 6) | (north << 4) | (west << 2) | south;
    }
    return 0;
}