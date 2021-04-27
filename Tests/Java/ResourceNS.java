import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.util.stream.Collectors;

class ResourceNS
{
    public static void main(String args[])
    {
        ResourceNS res = new ResourceNS();
        res.displayResourceText();
    }

    public void displayResourceText()
    {
        /*
         * Since Java SE 9, invoking getResourceXXX on a class in a named
         * module will only locate the resource in that module, it will
         * not search the class path as it did in previous release. So when
         * you use Class.getClassLoader().getResource() it will attempt to
         * locate the resource in the module containing the ClassLoader,
         * possibly something like:
         *      jdk.internal.loader.ClassLoaders.AppClassLoader
         * which is probably not the module that your resource is in, so it
         * returns null.
         *
         * You have to make java 9+ search for the file in your module.
         * Do that by changing Class to any class defined in your module in
         * order to make java use the proper class loader.
        */

        // Namespaces are relative, use leading '/' for full namespace
        InputStream is =
            ResourceNS.class.getResourceAsStream("/ns/ns1/HelloWorld.txt");

        InputStreamReader isr = new InputStreamReader(is);
        BufferedReader reader = new BufferedReader(isr);

        // lines() returns a collection convert it to a single string
        String out = reader.lines().collect(Collectors.joining(System.lineSeparator()));

        // Input text is one word per line, put it all on a single line.
        System.out.println(out.replace(System.lineSeparator(), " "));
    }
}
