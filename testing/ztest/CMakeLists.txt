
# ztest itself is pulled in by the application through CONFIG_ZTEST, so only
# the definitions header is contributed here.
gabs_implement(
    gabs-testing-ztest
    IMPLEMENTS
        gabs-testing
    INCLUDE
        .
)
