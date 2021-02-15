defmodule Mix.Tasks.Compile.Xz do
  def run(_) do
    if match? {:win32, _}, :os.type do
      IO.warn("Windows is not currently supported.")
      exit(1)
    else
      File.mkdir_p("priv")
      {result, _error_code} = System.cmd("make", [], stderr_to_stdout: true)
      IO.binwrite result
    end
    :ok
  end
end

defmodule XZ.MixProject do
  use Mix.Project

  def project do
    [
      app: :xz,
      compilers: [:xz] ++ Mix.compilers,
      version: "0.1.0",
      elixir: "~> 1.11",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      description: description(),
      package: package()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  def description do
    "Elixir NIF for the XZ data compression library. Requires liblzma to be installed on the target machine."
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:ex_doc, "~> 0.20.2", only: :dev}
    ]
  end

  def package do
    [
      name: "xz",
      maintainers: ["Warren Kenny"],
      licenses: ["MIT"],
      links: %{"GitHub" => "https://github.com/wrren/xz.ex"}
    ]
  end
end
