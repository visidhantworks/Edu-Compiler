FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-venv \
    gcc \
    flex \
    bison \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN pip3 install --break-system-packages -r requirements.txt

RUN make

EXPOSE 10000

CMD ["gunicorn", "--bind", "0.0.0.0:10000", "app:app"]