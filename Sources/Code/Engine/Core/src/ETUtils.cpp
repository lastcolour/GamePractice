namespace ET {

int GLOBAL_ET_ID = 0;

int GetNextETId() {
    ++GLOBAL_ET_ID;
    return GLOBAL_ET_ID;
}

} // namespace ET