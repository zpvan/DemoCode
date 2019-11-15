// Binderized == registerAsService

int main() 
{
    sp dumpstate = new DumpStateDevice;
    configureRpcThreadPool(1, true/* will join */);

    if (dumpstate->registerAsService() != OK) 
    {
        ALOGE("Could not register service.");
        return 1;
    }

    joinRpcThreadpool();
    ALOGE("Service exited!");
    return 1;

}