use math;


schema LampScreen
{
    _Width: int;
    _Height: int;
    _Buffer: bool[];

    // the bottom left of the screen
    _Location: location;
}
schema ColorScreen
{
    _Width: int;
    _Height: int;
    // on/off, colorID (blockID)
    _Buffer: iv2[];

    // the bottom left of the screen
    _Location: location;
}

Screen createScreen(dimensions: iv2, where: location, bool binary)
{

}