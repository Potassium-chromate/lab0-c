#!/usr/bin/python

import subprocess
import sys
import getopt


# Driver program for C programming exercise
class Tracer:

    traceDirectory = "./traces"
    qtest = "./qtest"
    command = qtest
    verbLevel = 0
    autograde = False
    useValgrind = False

    traceDict = {
        1: "trace-01-ops",
        2: "trace-02-ops",
        3: "trace-03-ops",
        4: "trace-04-ops",
        5: "trace-05-ops",
        6: "trace-06-string",
        7: "trace-07-robust",
        8: "trace-08-robust",
        9: "trace-09-robust",
        10: "trace-10-malloc",
        11: "trace-11-malloc",
        12: "trace-12-malloc",
        13: "trace-13-perf",
        14: "trace-14-perf",
        15: "trace-15-perf",
        16: "trace-16-perf",
        17: "trace-17-complexity"
    }

    traceProbs = {
        1: "Trace-01",
        2: "Trace-02",
        3: "Trace-03",
        4: "Trace-04",
        5: "Trace-05",
        6: "Trace-06",
        7: "Trace-07",
        8: "Trace-08",
        9: "Trace-09",
        10: "Trace-10",
        11: "Trace-11",
        12: "Trace-12",
        13: "Trace-13",
        14: "Trace-14",
        15: "Trace-15",
        16: "Trace-16",
        17: "Trace-17"
    }

    maxScores = [0, 6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5]

    def __init__(self,
                 qtest="",
                 verbLevel=0,
                 autograde=False,
                 useValgrind=False):
        if qtest != "":
            self.qtest = qtest
        self.verbLevel = verbLevel
        self.autograde = autograde
        self.useValgrind = useValgrind

    def runTrace(self, tid):
        if not tid in self.traceDict:
            print("ERROR: No trace with id %d" % tid)
            return False
        fname = "%s/%s.cmd" % (self.traceDirectory, self.traceDict[tid])
        vname = "%d" % self.verbLevel
        clist = [self.command, "-v", vname, "-f", fname]
        try:
            retcode = subprocess.call(" ".join(clist), shell=True)
        except Exception as e:
            print("Call of '%s' failed: %s" % (" ".join(clist), e))
            return False
        return retcode == 0

    def run(self, tid=0):
        scoreDict = {k: 0 for k in self.traceDict.keys()}
        print("---\tTrace\t\tPoints")
        if tid == 0:
            tidList = self.traceDict.keys()
        else:
            if not tid in self.traceDict:
                print("ERROR: Invalid trace ID %d" % tid)
                return
            tidList = [tid]
        score = 0
        maxscore = 0
        if self.useValgrind:
            self.command = 'valgrind ' + self.qtest
        else:
            self.command = self.qtest
        for t in tidList:
            tname = self.traceDict[t]
            if self.verbLevel > 0:
                print("+++ TESTING trace %s:" % tname)
            ok = self.runTrace(t)
            maxval = self.maxScores[t]
            tval = maxval if ok else 0
            print("---\t%s\t%d/%d" % (tname, tval, maxval))
            score += tval
            maxscore += maxval
            scoreDict[t] = tval
        print("---\tTOTAL\t\t%d/%d" % (score, maxscore))
        if self.autograde:
            # Generate JSON string
            jstring = '{"scores": {'
            first = True
            for k in scoreDict.keys():
                if not first:
                    jstring += ', '
                first = False
                jstring += '"%s" : %d' % (self.traceProbs[k], scoreDict[k])
            jstring += '}}'
            print(jstring)


def usage(name):
    print("Usage: %s [-h] [-p PROG] [-t TID] [-v VLEVEL] [--valgrind]" % name)
    print("  -h        Print this message")
    print("  -p PROG   Program to test")
    print("  -t TID    Trace ID to test")
    print("  -v VLEVEL Set verbosity level (0-3)")
    sys.exit(0)


def run(name, args):
    prog = ""
    tid = 0
    vlevel = 1
    levelFixed = False
    autograde = False
    useValgrind = False

    optlist, args = getopt.getopt(args, 'hp:t:v:A', ['valgrind'])
    for (opt, val) in optlist:
        if opt == '-h':
            usage(name)
        elif opt == '-p':
            prog = val
        elif opt == '-t':
            tid = int(val)
        elif opt == '-v':
            vlevel = int(val)
            levelFixed = True
        elif opt == '-A':
            autograde = True
        elif opt == '--valgrind':
            useValgrind = True
        else:
            print("Unrecognized option '%s'" % opt)
            usage(name)
    if not levelFixed and autograde:
        vlevel = 0
    t = Tracer(qtest=prog,
               verbLevel=vlevel,
               autograde=autograde,
               useValgrind=useValgrind)
    t.run(tid)


if __name__ == "__main__":
    run(sys.argv[0], sys.argv[1:])
