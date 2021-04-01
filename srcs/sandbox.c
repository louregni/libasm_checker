#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "test.h"
#include "display.h"
#include "unit-test.h"
#include "utils.h"

static int    extract_result(t_sandbox *sandbox, t_statement *statement)
{
    if (sandbox->pid)
    {
        if (waitpid(sandbox->pid, &sandbox->wstatus, WNOHANG) == sandbox->pid)
        {
            if (WIFEXITED(sandbox->wstatus))
                statement->terminate_code = sandbox->wstatus;
            else if (WIFSIGNALED(sandbox->wstatus))
                statement->terminate_code = WTERMSIG(sandbox->wstatus);
            sandbox->pid = 0;
            return 0;
        }
    }
    return 1;
}

void collect_results(t_core *core)
{
    t_function *unit;
    t_test *test;
    size_t total_tests = 0;
    size_t ended_tests = 0;

    usleep(100000);
    unit = (t_function *)core->ft_unit;
    for (size_t i = 0; i < NB_FUNCTION; ++i, ++unit)
    {
        total_tests += unit->nb_test;
        for (size_t c = 0; c < unit->nb_test; ++c)
        {
            test = (t_test *)&unit->test[c];
            if (!extract_result(&test->std_sandbox, &test->std_result) && \
                 !extract_result(&test->ft_sandbox, &test->ft_result))
            {
                unit->check(test);
                ++ended_tests;
            }
#ifdef DISPLAY
#endif
        }
    }

    int ko;
    unit = (t_function *)core->ft_unit;
    if (total_tests != ended_tests)
    {
        sleep(2);
        for (size_t i = 0; i < NB_FUNCTION; ++i, ++unit)
        {
            for (size_t c = 0; c < unit->nb_test; ++c)
            {
                test = (t_test *)&unit->test[c];

                if (test->valid != -1)
                    continue ;

                ko = 0;
                if (test->ft_sandbox.pid)
                {
                    if (extract_result(&test->ft_sandbox, &test->ft_result))
                    {
                        kill(test->ft_sandbox.pid, SIGTERM);
                        test->ft_result.terminate_code = SIGTERM;
                        ko = 1;
                    }
                }
                if (test->std_sandbox.pid)
                {
                    if (extract_result(&test->std_sandbox, &test->std_result))
                    {
                        kill(test->std_sandbox.pid, SIGTERM);
                        test->std_result.terminate_code = SIGTERM;
                        ko = 1;
                    }
                }
                if (!ko)
                    unit->check(test);
                else
                    test->valid = KO;
#ifdef DISPLAY
#endif
            }
        }
    }
}

