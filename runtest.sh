# pip uninstall -y helios 
# pip install -e ./
python setup.py build_ext --inplace install
python docs/tutorial/example.py
